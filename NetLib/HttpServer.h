// TODO: description

#pragma once

#ifndef NETLIB_HTTPSERVER_H
#define NETLIB_HTTPSERVER_H


#include <mutex>

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

		std::string m_receiveBuffer;

		std::mutex m_lockConnections;
		std::map<CLIENTID, HttpConnectionInfo> m_connections;

		HttpServerCb_RequestFromClient m_onRequestFromClient;

		
		void OnClientAccepted(const ClientInfo &pClientInfo);
		void OnClientDisconnected(const ClientInfo &pClientInfo);
		void OnReceivedFromClient(const ClientInfo &pClientInfo, char *pData, int pDataLength);

	};

} // ns NetLib


#endif // NETLIB_HTTPSERVER_H
