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

		inline static int GetBufferSize() { return (32 * 1024); }	// 32 KB. It is the maximum size of a package that can be sent or received

		NetErrCode CreateSocket(SOCKET &pSocket);
		NetErrCode SetOptReuseAddr(SOCKET &pSocket);
		NetErrCode SetOptNonBlock(SOCKET &pSocket);
		NetErrCode CloseSocket(SOCKET &pSocket);

		NetErrCode Send(SOCKET &pSocket, const char *pData, int pSizeInBytes);
	};

} // NetLib


#endif // NET_H
