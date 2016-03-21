#pragma once

#ifndef CLIENT_H
#define CLIENT_H


#include "Logger.h"
#include "NetErrCodes.h"
#include "Net.h"


namespace NetLib
{

	class Client : public Net
	{
	public:

		Client();
		virtual ~Client();

		NetErrCode Connect(std::string pServerIp, int pServerPort);
		NetErrCode Disconnect();

	protected:

		SOCKET m_sockClient;
	};

} // NetLib


#endif // CLIENT_H
