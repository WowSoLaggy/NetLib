// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Product:		NetLib
// Module:		Client
// Author:		Anton Egorov
// Description:	Class that connects to the server with
//				the given address, sends and receives data
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

#ifndef NETLIB_CLIENT_H
#define NETLIB_CLIENT_H


#include <Windows.h>
#include <functional>
#include <string>
#include <vector>
#include <atomic>

#include "NetErrCodes.h"


namespace NetLib
{
	using namespace NetErrCodes;


	// Typedef for the data received callback
	typedef std::function<void(char *pData, int pDataLength)> ClientCb_ReceivedFromServer;


	// Class that connects to the server with
	// the given address, sends and receives data
	class Client
	{
	public:

		// Creates a new client instance
		// Params:
		// [in] ClientCb_ReceivedFromServer pOnReceivedFromServer	- callback that is called when the new data is received from the server
		Client(ClientCb_ReceivedFromServer pOnReceivedFromServer);

		//
		virtual ~Client();


		// Connects to the remote server with the given address
		// Params:
		// [in] const std::string & pServerIp	- string that represents the remote server address
		// [in] int pServerPort					- remote server port
		NetErrCode Connect(const std::string &pServerIp, int pServerPort);

		// Disconnects from the remote server
		NetErrCode Disconnect();
		

		// Sends the given data to the remote server
		// Params:
		// [in] const char * pData	- data to send to the remote server
		// [in] int pSizeInBytes	- size of the data to be sent
		NetErrCode SendToServer(const char *pData, int pSizeInBytes);

	protected:

		std::atomic_bool m_isRunning;						// Variable used to stop the client
		SOCKET m_sockClient;								// Socket to connect to the remote server
		std::vector<char> m_receiveBuffer;					// Buffer to receive data to

		ClientCb_ReceivedFromServer m_onReceivedFromServer;	// Callback that is called when the new data is received from the server


		// Main loop that repeats until the client is stopped
		void MainLoop();

		// Tries to receive data from the remote server if any
		void TryReceive();

	};

} // ns NetLib


#endif // NETLIB_CLIENT_H
