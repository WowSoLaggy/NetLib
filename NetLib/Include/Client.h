// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Product:		NetLib
// Module:		Client
// Author:		Anton Egorov
// Description:	Class that connects to the server with
//				the given address, sends and receives data
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

#ifndef CLIENT_H
#define CLIENT_H


#include "Logger.h"
#include "NetErrCodes.h"
#include "Net.h"


namespace NetLib
{

	// Class that connects to the server with
	// the given address, sends and receives data
	class Client
	{
	public:

		// Default ctor
		Client();

		// Default dtor
		virtual ~Client();


		// Connects to the remote server with the given address
		// Params:
		// [in] std::string pServerIp	- string that represents the remote server address
		// [in] int pServerPort			- remote server port
		NetErrCode Connect(std::string pServerIp, int pServerPort);

		// Disconnects from the remote server
		NetErrCode Disconnect();

		
		// Sends the given data to the remote server
		// Params:
		// [in] const char * pData	- data to send to the remote server
		// [in] int pSizeInBytes	- size of the data to be sent
		NetErrCode Send(const char *pData, int pSizeInBytes);

		// Tries to receive data from the remote server if any
		// Params:
		// [in]  char * pBuffer				- buffer to receive data to
		// [in]  int pBufferSize			- length of the buffer to receive data to
		// [out] int & pReceivedDataLength	- length of the received data (can be 0 if nothing to receive)
		NetErrCode Receive(char *pBuffer, int pBufferSize, int &pReceivedDataLength);

	protected:

		SOCKET m_sockClient;	// Socket to connect to the remote server
	};

} // NetLib


#endif // CLIENT_H
