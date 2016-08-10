// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Product:		NetLib
// Module:		HttpServer
// Author:		Anton Egorov
// Description:	Class that controlls the HTTP server. It's 
//				configuration is taken from the config file
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

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
#include "Auth.h"


namespace NetLib
{

	// Typedef for the request received callback
	typedef std::function<void(const HttpConnectionInfo &pHttpConnectionInfo, const HttpRequest &pRequest)> HttpServerCb_RequestFromClient;


	// Class that represents the HttpServer
	class HttpServer : protected Server
	{
	public:

		HttpServer(HttpServerCb_RequestFromClient pOnRequestFromClient);
		virtual ~HttpServer();

		
		// Starts the Http Server
		// Params:
		// [in] const std::string & pConfigFilePath	- path to the config file
		NetErrCode Start(const std::string &pConfigFilePath);

		// Stops the Http Server
		NetErrCode Stop();

		
		// Sends the given Http Response to the client with the given Id
		// Params:
		// [in] CLIENTID pClientId				- id of the client to send the response to
		// [in] HttpResponse & pHttpResponse	- response to send to the client with the given id
		NetErrCode SendToClient(CLIENTID pClientId, HttpResponse & pHttpResponse);

	private:

		std::atomic_bool m_isRunning;							// Flag that indicates whether the HttpServer is running
		std::string m_receiveBuffer;							// Buffer to store received data

		std::mutex m_lockRequests;								// Mutex to lock access to the queue of requests
		std::queue<std::tuple<NetErrCode, HttpConnectionInfo, HttpRequest>> m_requests;	// Queue of requests to process

		std::mutex m_lockConnections;							// Mutex to lock the list of current connections
		std::vector<HttpConnectionInfo> m_connections;			// List of current connections

		HttpServerCb_RequestFromClient m_onRequestFromClient;	// Callback that is called when a new request from the client should be processed by the control application

		Auth m_authorizer;										// Object used to authorize incoming requests

		
		// Callback to handle accepted clients
		void OnClientAccepted(const ClientInfo &pClientInfo);

		// Callback to handle client disconnect
		void OnClientDisconnected(const ClientInfo &pClientInfo);

		// Callback to handle received data from the client
		void OnReceivedFromClient(const ClientInfo &pClientInfo, char *pData, int pDataLength);


		// Consumer thread that generates callback to the control application
		void MainLoop();

		// Handles requests to files (put, delete, get, ...)
		NetErrCode FileHandler(const HttpConnectionInfo &pHttpConnectionInfo, const HttpRequest &pHttpRequest);

	};

} // ns NetLib


#endif // NETLIB_HTTPSERVER_H
