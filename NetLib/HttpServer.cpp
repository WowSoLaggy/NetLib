#include "HttpServer.h"


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


	NetErrCode HttpServer::Start(int pPort)
	{
		return Server::Start(pPort);
	}

	NetErrCode HttpServer::Stop()
	{
		return Server::Stop();
	}


	void HttpServer::OnClientAccepted(const ClientInfo &pClientInfo)
	{
	}

	void HttpServer::OnClientDisconnected(const ClientInfo &pClientInfo)
	{
	}

	void HttpServer::OnReceivedFromClient(const ClientInfo &pClientInfo, char *pData, int pDataLength)
	{
		NetErrCode err;
		m_receiveBuffer.append(pData, pDataLength);

		int processedChars = 0;
		HttpRequest request;
		err = request.Parse(m_receiveBuffer, processedChars);
		if (err != neterr_noErr)
		{
			// TODO: handle err, disconnect
			return;
		}

		if (m_onRequestFromClient != nullptr)
			m_onRequestFromClient(pClientInfo, request);
	}

} // ns NetLib
