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
		// [in] size_t pSizeInBytes	- size of the data to be sent
		NetErrCode Send(const char *pData, size_t pSizeInBytes);

		// Tries to receive data from the remote server if any
		// Params:
		// [in]  char * pBuffer				- buffer to receive data to
		// [in]  size_t pBufferSize			- length of the buffer to receive data to
		// [out] int & pReceivedDataLength	- length of the received data (can be 0 if nothing to receive)
		NetErrCode Receive(char *pBuffer, size_t pBufferSize, int &pReceivedDataLength);

	protected:

		SOCKET m_sockClient;	// Socket to connect to the remote server
	};

} // NetLib


#endif // NETLIB_CLIENT_H
