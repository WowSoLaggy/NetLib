#include "HttpClient.h"

#include "Log.h"


namespace NetLib
{

	HttpClient::HttpClient(HttpClientCb_ResponseFromServer pOnResponseFromServer)
		: Client(std::bind(&HttpClient::OnReceivedFromServer, this, std::placeholders::_1, std::placeholders::_2)),
		m_onResponseFromServer(pOnResponseFromServer)
	{
	}

	HttpClient::~HttpClient()
	{
	}


	NetErrCode HttpClient::SendRequest(const HttpRequest &pHttpRequest, const std::string &pAddress, int pPort)
	{
		LOG("HttpClient::SendRequest()");
		NetErrCode err;

		std::string address = m_bindedAddress;
		int port = m_bindedPort;

		if (!pAddress.empty())
		{
			address = pAddress;
			port = pPort;
		}

		if (address.empty() || port < 0 || port > std::numeric_limits<uint16_t>::max())
		{
			echo("ERROR: Invalid address specified to send the request to.");
			return neterr_invalidAddress;
		}

		err = Connect(address, port);
		if (err != neterr_noErr)
		{
			echo("ERROR: Can't connect to the remote server.");
			return err;
		}

		std::string requestString = pHttpRequest.ToString(true);
		err = Client::SendToServer(requestString.c_str(), requestString.size());
		if (err != neterr_noErr)
		{
			echo("ERROR: Can't send message to the server.");
			return err;
		}

		return neterr_noErr;
	}


	NetErrCode HttpClient::BindAddress(const std::string &pAddress, int pPort)
	{
		m_bindedAddress = pAddress;
		m_bindedPort = pPort;
		return neterr_noErr;
	}


	void HttpClient::OnReceivedFromServer(char *pData, int pDataLength)
	{
		LOG("HttpClient::OnReceivedFromServer()");
		NetErrCode err;

		int offset = 0;
		HttpResponse httpResponse;
		err = httpResponse.Parse(std::string(pData, pDataLength), offset);
		if (err != neterr_noErr)
		{
			echo("ERROR: Can't parse response.");
			return;
		}

		if (m_onResponseFromServer != nullptr)
			m_onResponseFromServer(httpResponse);

		err = Disconnect();
		if (err != neterr_noErr)
		{
			echo("ERROR: Can't close the connection.");
			return;
		}
	}

} // ns NetLib
