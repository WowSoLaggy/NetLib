// TODO: description

#pragma once

#ifndef NETLIB_HTTPSERVER_H
#define NETLIB_HTTPSERVER_H


#include "Server.h"


namespace NetLib
{

	typedef std::function<void()> HttpServer_ClientAccepted;


	class HttpServer : public Server
	{
	public:

		HttpServer();

		virtual ~HttpServer();

	private:

		void OnClientAccepted(CLIENTID pClientId, const std::string &pClientAddress, int pClientPort);
		void OnClientDisconnected(CLIENTID pClientId);
		void OnReceivedFromClient(CLIENTID pClientId, char *pData, int pDataLength);

	};

} // ns NetLib


#endif // NETLIB_HTTPSERVER_H
