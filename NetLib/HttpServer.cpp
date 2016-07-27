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


	NetErrCode HttpServer::SendToClient(CLIENTID pClientId, const HttpResponse & pHttpResponse)
	{
		std::string responseString = pHttpResponse.ToString();
		return Server::SendToClient(pClientId, responseString.data(), responseString.size());
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

		if (processedChars <= 0)
			m_receiveBuffer.clear();
		else if (processedChars > 0)
			m_receiveBuffer.erase(0, processedChars);

		if (err != neterr_noErr)
		{
			if (err == neterr_parse_requestLineTooLong)
				SendToClient(pClientInfo.Id, HttpResponse::RequestUrlTooLong(true));
			else
				SendToClient(pClientInfo.Id, HttpResponse::BadRequest(true));

			return;
		}

		if (m_onRequestFromClient != nullptr)
			m_onRequestFromClient(pClientInfo, request);
	}

} // ns NetLib
