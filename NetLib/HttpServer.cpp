#include "HttpServer.h"


namespace NetLib
{

	HttpServer::HttpServer()
		: Server(
			std::bind(&HttpServer::OnClientAccepted, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
			std::bind(&HttpServer::OnClientDisconnected, this, std::placeholders::_1),
			std::bind(&HttpServer::OnReceivedFromClient, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
	{
	}

	HttpServer::~HttpServer()
	{
	}


	void HttpServer::OnClientAccepted(CLIENTID pClientId, const std::string &pClientAddress, int pClientPort)
	{
	}

	void HttpServer::OnClientDisconnected(CLIENTID pClientId)
	{
	}

	void HttpServer::OnReceivedFromClient(CLIENTID pClientId, char *pData, int pDataLength)
	{
	}

} // ns NetLib
