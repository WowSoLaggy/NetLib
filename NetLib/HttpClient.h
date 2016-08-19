// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Product:		NetLib
// Module:		HttpClient
// Author:		Anton Egorov
// Description:	Class that controls the HTTP client
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

#ifndef NETLIB_HTTPCLIENT_H
#define NETLIB_HTTPCLIENT_H


#include <string>

#include "Client.h"
#include "HttpRequest.h"
#include "HttpResponse.h"


namespace NetLib
{

	// Typedef for the response received callback
	typedef std::function<void(const HttpResponse &pResponse)> HttpClientCb_ResponseFromServer;


	// Http client class
	class HttpClient : private Client
	{
	public:

		HttpClient(HttpClientCb_ResponseFromServer pOnResponseFromServer = nullptr);
		virtual ~HttpClient();


		// Sends the given request to the given address. If no address specified the request will be sent to the binded address
		// Params:
		// [in] const HttpRequest & pHttpRequest	- request to send
		// [in] const std::string & pAddress		- address to send the given request to
		// [in] int pPort							- port to send the given request to
		NetErrCode SendRequest(const HttpRequest &pHttpRequest, const std::string &pAddress = "", int pPort = 80);

		// Bind client to the given address. All outgoing requests without address specified will be sent to the binded address
		// Params:
		// [in] const std::string & pAddress	- address to bind outgoing requests to. Can be empty to unbind the client from any address
		// [in] int pPort						- port to bind all outgoing requests to
		NetErrCode BindAddress(const std::string &pAddress = "", int pPort = 80);


	private:

		std::string m_bindedAddress;							// Address to bind outgoing requests to
		int m_bindedPort;										// Port to bind outgoing requests to

		HttpClientCb_ResponseFromServer m_onResponseFromServer;	// Callback that is called when a new response from the server should be processed by the control application


		// Callback to handle received data from the server
		void OnReceivedFromServer(char *pData, int pDataLength);

	};

} // ns NetLib


#endif // NETLIB_HTTPCLIENT_H
