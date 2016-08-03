// TODO: description

#pragma once

#ifndef NETLIB_HTTPSERVER_H
#define NETLIB_HTTPSERVER_H


#include <mutex>
#include <queue>

#include "Config.h"
#include "Server.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpConnectionInfo.h"


namespace NetLib
{

	// Typedef for the request received callback
	typedef std::function<void(const ClientInfo &pClientInfo, const HttpRequest &pRequest)> HttpServerCb_RequestFromClient;


	class HttpServer : protected Server
	{
	public:

		HttpServer(HttpServerCb_RequestFromClient pOnRequestFromClient);
		virtual ~HttpServer();

		NetErrCode Start();
		NetErrCode Stop();

		NetErrCode SendToClient(CLIENTID pClientId, HttpResponse & pHttpResponse);

	private:

		std::atomic_bool m_isRunning;
		std::string m_receiveBuffer;

		std::mutex m_lockRequests;
		std::queue<std::tuple<NetErrCode, ClientInfo, HttpConnectionInfo, HttpRequest>> m_requests;

		std::mutex m_lockConnections;
		std::vector<HttpConnectionInfo> m_connections;

		HttpServerCb_RequestFromClient m_onRequestFromClient;

		
		void OnClientAccepted(const ClientInfo &pClientInfo);
		void OnClientDisconnected(const ClientInfo &pClientInfo);
		void OnReceivedFromClient(const ClientInfo &pClientInfo, char *pData, int pDataLength);


		void MainLoop();

	};

} // ns NetLib


#endif // NETLIB_HTTPSERVER_H
