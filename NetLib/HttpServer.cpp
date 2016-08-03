#include "HttpServer.h"

#include <algorithm>

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


	NetErrCode HttpServer::Start(const std::string &pConfigFilePath)
	{
		LOG("HttpServer::Start()");
		NetErrCode err;

		err = Config::ReadFromFile(pConfigFilePath);
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

		// Start the consumer thread. The Server's thread is a producer

		m_isRunning = true;
		std::thread consumerThread(&HttpServer::MainLoop, this);
		consumerThread.detach();

		return neterr_noErr;
	}

	NetErrCode HttpServer::Stop()
	{
		m_isRunning = false;
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

				m_connections.emplace_back(HttpConnectionInfo(pClientInfo.Id));

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
		LOG("HttpServer::OnClientDisconnected()");

		std::unique_lock<std::mutex> lock(m_lockConnections);

		auto it = std::find_if(m_connections.begin(), m_connections.end(), [&](auto &it) { return it.Id == pClientInfo.Id; });
		if (it == m_connections.end())
		{
			// Client disconnected, drop the request
			echo("ERROR: Can't find info about the request's client (id: ", pClientInfo.Id, "). Ignore.");
			return;
		}

		m_connections.erase(it);
	}

	void HttpServer::OnReceivedFromClient(const ClientInfo &pClientInfo, char *pData, int pDataLength)
	{
		LOG("HttpServer::OnReceivedFromClient()");
		NetErrCode err;

		// Find info about the client

		HttpConnectionInfo httpConnectionInfo;
		{
			std::unique_lock<std::mutex> lock(m_lockConnections);
			auto it = std::find_if(m_connections.begin(), m_connections.end(), [&](auto &it) { return it.Id == pClientInfo.Id; });
			if (it == m_connections.end())
			{
				// Client disconnected, drop the request
				echo("ERROR: Can't find info about the request's client (id: ", pClientInfo.Id, "). Ignore.");
				return;
			}
			httpConnectionInfo = *it;
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

		// Add the parse result and the request to the request queue

		{
			std::unique_lock<std::mutex> lock(m_lockRequests);
			m_requests.push({ err, pClientInfo, httpConnectionInfo, request });
		}
	}


	void HttpServer::MainLoop()
	{
		LOG("HttpServer::MainLoop()");
		NetErrCode err;

		ClientInfo clientInfo;
		HttpConnectionInfo httpConnectionInfo;
		HttpRequest request;
		while (m_isRunning)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));

			// Check the request queue
			{
				std::unique_lock<std::mutex> lock(m_lockRequests);
				if (m_requests.size() == 0)
					continue;

				std::tie(err, clientInfo, httpConnectionInfo, request) = m_requests.front();
				m_requests.pop();
			}

			// Check whether we should auto generate the response or just pass the request to the control application

			if (err != neterr_noErr)
			{
				HttpResponse httpResponse;

				if (err == neterr_parse_requestLineTooLong)
					httpResponse = HttpResponse::RequestUrlTooLong();
				else if (err == neterr_parse_methodNotAllowed)
					httpResponse = HttpResponse::MethodNotAllowed();
				else
					httpResponse = HttpResponse::BadRequest();

				err = SendToClient(httpConnectionInfo.Id, httpResponse);
				if (err != neterr_noErr)
					echo("ERROR: Can't send the response to the client (id: ", httpConnectionInfo.Id, ").");
			}
			else
			{
				// Should we handle requests to the file system or pass it to the control application?
				if ((Config::GetAllowFileHandle()) && (
					(request.GetMethod() == req_get) || 
					(request.GetMethod() == req_head) ||
					(request.GetMethod() == req_put) || 
					(request.GetMethod() == req_delete)
					))
				{
					err = FileHandler(clientInfo, request);
					if (err != neterr_noErr)
						echo("ERROR: Can't handle request to the file system.");
				}
				else
				{
					if (m_onRequestFromClient != nullptr)
						m_onRequestFromClient(clientInfo, request);
				}
			}

			// Check whether we should disconnect the client

			std::string str = request.GetHeaders()["Connection"];
			std::transform(str.begin(), str.end(), str.begin(), ::tolower);
			bool forceDisconnect = (
				(!Config::GetKeepAliveSupport()) ||												// we do not support keep-alive
				(str.compare("keep-alive") != 0) ||												// client forces not "keep-alive"
				(httpConnectionInfo.GetConnectionTime() >= Config::GetKeepAliveTimeout()) ||	// client's timeout elapsed
				(httpConnectionInfo.RequestsCount >= Config::GetKeepAliveMaxRequests()));		// client's number of requests exceeded
			if (forceDisconnect)
			{
				err = DisconnectClient(clientInfo.Id);
				if (err != neterr_noErr)
					echo("ERROR: Can't disconnect client (id: ", clientInfo.Id, ").");
			}
		}

		// Clear info about all connected client's
		m_connections.clear();

		err = Server::Stop();
		if (err != neterr_noErr)
		{
			echo("ERROR: Can't stop the web server. Error code: ", err, ".");
			return;
		}
	}

} // ns NetLib
