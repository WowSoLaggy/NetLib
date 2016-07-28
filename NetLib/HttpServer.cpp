#include "HttpServer.h"

#include "Utils.h"
#include "Net.h"


namespace NetLib
{

	HttpServer::HttpServer(HttpServerCb_RequestFromClient pOnRequestFromClient)
		: Server(
			std::bind(&HttpServer::OnClientAccepted, this, std::placeholders::_1),
			std::bind(&HttpServer::OnClientDisconnected, this, std::placeholders::_1),
			std::bind(&HttpServer::OnReceivedFromClient, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)),
		m_onRequestFromClient(pOnRequestFromClient)
	{
	}

	HttpServer::~HttpServer()
	{
	}


	NetErrCode HttpServer::Start()
	{
		NetErrCode err = Config::ReadFromFile(Config::GetConfigFileName());
		return Server::Start(Config::GetServerPort());
	}

	NetErrCode HttpServer::Stop()
	{
		m_connections.clear();
		return Server::Stop();
	}


	NetErrCode HttpServer::SendToClient(CLIENTID pClientId, HttpResponse &pHttpResponse)
	{
		if (Config::GetAppendDateTimeStamp())
			pHttpResponse.AddHeader("Date", GetHttpDate());
		if (Config::GetAppendServerName())
			pHttpResponse.AddHeader("Server", Config::GetServerName());
		

		std::string responseString = pHttpResponse.ToString();
		return Server::SendToClient(pClientId, responseString.data(), responseString.size());
	}


	void HttpServer::OnClientAccepted(const ClientInfo &pClientInfo)
	{
		{
			std::unique_lock<std::mutex> lock(m_lockConnections);
			if ((int)m_connections.size() < Config::GetMaxConnections())
				m_connections.insert({ pClientInfo.Id, HttpConnectionInfo() });
			return;
		}

		// We don't need lock to disconnect client

		DisconnectClient(pClientInfo.Id);
	}

	void HttpServer::OnClientDisconnected(const ClientInfo &pClientInfo)
	{
		std::unique_lock<std::mutex> lock(m_lockConnections);
		m_connections.erase(pClientInfo.Id);
	}

	void HttpServer::OnReceivedFromClient(const ClientInfo &pClientInfo, char *pData, int pDataLength)
	{
		NetErrCode err;
		NetErrCode err2;

		// Get info about the client

		HttpConnectionInfo httpConnectionInfo;
		{
			std::unique_lock<std::mutex> lock(m_lockConnections);
			auto it = m_connections.find(pClientInfo.Id);
			if (it == m_connections.end())
			{
				// Client disconnected, ignore
				return;
			}
			httpConnectionInfo = it->second;
		}
		++httpConnectionInfo.RequestsCount;

		// Client is active

		m_receiveBuffer.append(pData, pDataLength);

		int processedChars = 0;
		HttpRequest request;
		err = request.Parse(m_receiveBuffer, processedChars);

		// Removed parsed data from the receive buffer

		if (processedChars <= 0)
			m_receiveBuffer.clear();
		else if (processedChars > 0)
			m_receiveBuffer.erase(0, processedChars);

		// Check whether we should auto generate the response or just pass the request to the control application

		if (err != neterr_noErr)
		{
			if (err == neterr_parse_requestLineTooLong)
				err2 = SendToClient(pClientInfo.Id, HttpResponse::RequestUrlTooLong());
			else if (err == neterr_parse_methodNotAllowed)
				err2 = SendToClient(pClientInfo.Id, HttpResponse::MethodNotAllowed());
			else
				err2 = SendToClient(pClientInfo.Id, HttpResponse::BadRequest());

			if (err2 != neterr_noErr)
			{
				// TODO: Log error
			}
		}
		else
		{
			if (m_onRequestFromClient != nullptr)
				m_onRequestFromClient(pClientInfo, request);
		}

		// Check whether we should disconnect the client

		std::string str = request.GetHeaders()["Connection"];
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		bool forceDisconnect = (!Config::GetKeepAliveSupport()) || (str.compare("keep-alive") != 0);
		forceDisconnect = ((forceDisconnect) ||
			(httpConnectionInfo.ConnectTimer.Check() >= Config::GetKeepAliveTimeout()) ||
			(httpConnectionInfo.RequestsCount >= Config::GetKeepAliveMaxRequests()));
		if (forceDisconnect)
		{
			// TODO:
			// That's a big workaround shit, that should be removed as soon as possible.
			// We just make the socket invalid, so that it will be disconnected on the next Server's select().
			// If we will call DisconnectClient here it will corrupt Server's loop of recv
			// Until it is fixed, the control application can't disconnect clients from this callback.
			shutdown(pClientInfo.Id, SD_BOTH);
		}
	}

} // ns NetLib
