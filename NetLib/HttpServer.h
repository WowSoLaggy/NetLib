// TODO: description

#pragma once

#ifndef NETLIB_HTTPSERVER_H
#define NETLIB_HTTPSERVER_H


#include "Server.h"
#include "HttpRequest.h"


namespace NetLib
{

	// Typedef for the request received callback
	typedef std::function<void(const ClientInfo &pClientInfo, const HttpRequest &pRequest)> HttpServerCb_RequestFromClient;


	class HttpServer : protected Server
	{
	public:

		HttpServer(HttpServerCb_RequestFromClient pOnRequestFromClient);
		virtual ~HttpServer();

		NetErrCode Start(int pPort = 80);
		NetErrCode Stop();

	private:

		std::string m_receiveBuffer;

		HttpServerCb_RequestFromClient m_onRequestFromClient;

		void OnClientAccepted(const ClientInfo &pClientInfo);
		void OnClientDisconnected(const ClientInfo &pClientInfo);
		void OnReceivedFromClient(const ClientInfo &pClientInfo, char *pData, int pDataLength);

	};

} // ns NetLib


#endif // NETLIB_HTTPSERVER_H
