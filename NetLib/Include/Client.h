#pragma once

#ifndef CLIENT_H
#define CLIENT_H


#include "Logger.h"
#include "NetErrCodes.h"
#include "Net.h"


namespace NetLib
{
	class Client
	{
	public:

		Client();
		virtual ~Client();

		NetErrCode Connect(std::string pServerIp, int pServerPort);
		NetErrCode Disconnect();

		NetErrCode Send(const char *pData, int pSizeInBytes);
		NetErrCode Receive(char *pBuffer, int pBufferSize, int &pReceivedDataLength);

	protected:

		SOCKET m_sockClient;
	};

} // NetLib


#endif // CLIENT_H
