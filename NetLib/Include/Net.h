#pragma once

#ifndef NET_H
#define NET_H


#include "Logger.h"
#include "NetErrCodes.h"


#define NETINIT Net::Init()
#define NETDISPOSE Net::Dispose()


namespace NetLib
{

	class Net
	{
	public:

		static NetErrCode Init();
		static NetErrCode Dispose();

	protected:

		NetErrCode CreateSocket(SOCKET &pSocket);
		NetErrCode SetOptReuseAddr(SOCKET &pSocket);
		NetErrCode SetOptNonBlock(SOCKET &pSocket);
		NetErrCode CloseSocket(SOCKET pSocket);
	};

} // NetLib


#endif // NET_H
