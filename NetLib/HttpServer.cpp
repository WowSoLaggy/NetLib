#include "HttpServer.h"

#include "Log.h"
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
		LOG("HttpServer::Start()");
		NetErrCode err;

		err = Config::ReadFromFile(Config::GetConfigFileName());
		if (err != neterr_noErr)
		{
			echo("ERROR: Error occured while parsing the config file.");
			return err;
		}

		err = Server::Start(Config::GetServerPort());
		if (err != neterr_noErr)
		{
			echo("ERROR: Error occured while starting the web server.");
			return err;
		}

		return neterr_noErr;
	}

	NetErrCode HttpServer::Stop()
	{
		LOG("HttpServer::Stop()");
		NetErrCode err;

		// Clear info about all connected client's
		m_connections.clear();

		err = Server::Stop();
		if (err != neterr_noErr)
		{
			echo("ERROR: Error occured while stopping the web server.");
			return err;
		}

		return neterr_noErr;
	}


	NetErrCode HttpServer::SendToClient(CLIENTID pClientId, HttpResponse &pHttpResponse)
	{
		LOG("HttpServer::SendToClient()");
		NetErrCode err;

		// Append DateTime and Version if needed

		if (Config::GetAppendDateTimeStamp())
			pHttpResponse.AddHeader("Date", GetHttpDate());
		if (Config::GetAppendServerName())
			pHttpResponse.AddHeader("Server", Config::GetServerName());

		// Send this response to the client as a string

		std::string responseString = pHttpResponse.ToString();
		err = Server::SendToClient(pClientId, responseString.data(), responseString.size());
		if (err != neterr_noErr)
		{
			echo("ERROR: Can't send the response to the client (id: ", pClientId, ").");
			return err;
		}

		return neterr_noErr;
	}


	void HttpServer::OnClientAccepted(const ClientInfo &pClientInfo)
	{
		LOG("HttpServer::OnClientAccepted()");
		NetErrCode err;

		{
			std::unique_lock<std::mutex> lock(m_lockConnections);

			// Check number of current connections

			if ((int)m_connections.size() < Config::GetMaxConnections())
			{
				// We can connect the client

				m_connections.insert({ pClientInfo.Id, HttpConnectionInfo() });

				if (Config::GetLogOnAccept())
					echo("Accepted client: ", pClientInfo.ToString(), ".");

				return;
			}
		}

		// TOO MUCH CLIENTS! Kick him.
		// No need to lock to disconnect client

		if (Config::GetLogOnAccept())
			echo("Can't accept client: ", pClientInfo.ToString(), ". Too many connections.");

		err = DisconnectClient(pClientInfo.Id);
		if (err != neterr_noErr)
		{
			echo("ERROR: Can't disconnect the client (id: ", pClientInfo.Id, ").");
			return;
		}
	}

	void HttpServer::OnClientDisconnected(const ClientInfo &pClientInfo)
	{
		std::unique_lock<std::mutex> lock(m_lockConnections);
		m_connections.erase(pClientInfo.Id);
	}

	void HttpServer::OnReceivedFromClient(const ClientInfo &pClientInfo, char *pData, int pDataLength)
	{
		LOG("HttpServer::OnReceivedFromClient()");
		NetErrCode err;
		NetErrCode err2;

		// Find info about the client

		HttpConnectionInfo httpConnectionInfo;
		{
			std::unique_lock<std::mutex> lock(m_lockConnections);
			auto it = m_connections.find(pClientInfo.Id);
			if (it == m_connections.end())
			{
				// Client disconnected, drop the request
				echo("ERROR: Can't find info about the request's client (id: ", pClientInfo.Id, "). Ignore.");
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
				echo("ERROR: Can't send the response to the client (id: ", pClientInfo.Id, ").");
		}
		else
		{
			if (m_onRequestFromClient != nullptr)
				m_onRequestFromClient(pClientInfo, request);
		}

		// Check whether we should disconnect the client

		std::string str = request.GetHeaders()["Connection"];
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		bool forceDisconnect = (
			(!Config::GetKeepAliveSupport()) ||												// we do not support keep-alive
			(str.compare("keep-alive") != 0) ||												// client forces not "keep-alive"
			(httpConnectionInfo.ConnectTimer.Check() >= Config::GetKeepAliveTimeout()) ||	// client's timeout elapsed
			(httpConnectionInfo.RequestsCount >= Config::GetKeepAliveMaxRequests()));		// client's number of requests exceeded
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
