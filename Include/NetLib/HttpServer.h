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

		HttpServer()
			: Server(
				std::bind(&HttpServer::OnClientAccepted, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
				std::bind(&HttpServer::OnClientDisconnected, this, std::placeholders::_1),
				std::bind(&HttpServer::OnReceivedFromClient, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
		{
		}

		virtual ~HttpServer()
		{
		}

	private:

		void OnClientAccepted(CLIENTID pClientId, const std::string &pClientAddress, int pClientPort)
		{
		}

		void OnClientDisconnected(CLIENTID pClientId)
		{
		}

		void OnReceivedFromClient(CLIENTID pClientId, char *pData, int pDataLength)
		{
		}

	};

} // ns NetLib


#endif // NETLIB_HTTPSERVER_H
