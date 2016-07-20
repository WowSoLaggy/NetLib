// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Product:		NetLib
// Module:		Net
// Author:		Anton Egorov
// Description:	Class that contains common network
//				functions for client and server
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

#ifndef NETLIB_NET_H
#define NETLIB_NET_H


#include <Windows.h>
#include <string>
#include "Log.h"

#include "NetErrCodes.h"

#pragma comment(lib, "Ws2_32.lib") // Win socket library


// Manifest constant for shutdown()
#define SD_BOTH	0x02


namespace NetLib
{
	using namespace NetLib::NetErrCodes;

	// Class that contains common network functions for client and server
	class Net
	{
	public:

		// Static function for common network initialization
		static NetErrCode Init()
		{
			LOG("Net::Init()");
			int res;

			// Initialize socket library

			WSADATA wsaData;
			res = WSAStartup(MAKEWORD(2, 2), &wsaData);
			if (res != 0)
			{
				echo("ERROR: Can't initialize socket library.");
				return neterr_noWsa;
			}

			return neterr_noErr;
		}


		// Static function for common network disposion
		static NetErrCode Dispose()
		{
			LOG("Net::Dispose()");
			int res;

			// Clean up socket library

			res = WSACleanup();
			if (res != 0)
			{
				echo("ERROR: Can't dispose socket library");
				return neterr_cantCleanupWsa;
			}

			return neterr_noErr;
		}



		// Returns the maximum size of a package that can be sent or received.
		// If the package to be sent is larger then it will be divided
		inline static int GetBufferSize() { return (32 * 1024); }



		// Creates a new socket
		// Params:
		// [out] SOCKET & pSocket	- reference to the socket to be create
		static NetErrCode CreateSocket(SOCKET &pSocket)
		{
			LOG("Net::CreateSocket()");

			pSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (pSocket == INVALID_SOCKET)
			{
				echo("ERROR: Can't create socket.");
				return neterr_cantCreateSocket;
			}

			return neterr_noErr;
		}


		// Sets the SO_REUSEADDR option for the given socket
		// Params:
		// [in] SOCKET & pSocket	- reference to the socket to set option to
		static NetErrCode SetOptReuseAddr(SOCKET &pSocket)
		{
			LOG("Net::SetOptReuseAddr()");
			int res;

			int int_on = 1;
			res = setsockopt(pSocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&int_on, sizeof(int_on));
			if (res != 0)
			{
				echo("ERROR: Can't set socket SO_REUSEADDR option.");
				return neterr_cantSetSocketOptions;
			}

			return neterr_noErr;
		}

		
		// Sets the NONBLOCK option for the given socket
		// Params:
		// [in] SOCKET & pSocket	- reference to the socket to set option to
		// [in] bool pOn			- turn the option on or off
		static NetErrCode SetOptNonBlock(SOCKET &pSocket, bool pOn)
		{
			LOG("Net::SetOptNonBlock()");
			int res;

			u_long u_long_on = pOn ? 1 : 0;
			res = ioctlsocket(pSocket, FIONBIO, &u_long_on);
			if (res != 0)
			{
				echo("ERROR: Can't set socket non-blocking mode.");
				return neterr_cantSetSocketOptions;
			}

			return neterr_noErr;
		}


		// Closes the given socket
		// Params:
		// [in] SOCKET & pSocket	- reference to the socket to close
		static NetErrCode CloseSocket(SOCKET &pSocket)
		{
			shutdown(pSocket, SD_BOTH);
			closesocket(pSocket);

			return neterr_noErr;
		}



		// Sends the given data to the given socket
		// Params:
		// [in] SOCKET & pSocket	- socket to send data to
		// [in] const char * pData	- data to be sent
		// [in] int pSizeInBytes	- size of data to be sent in bytes
		static NetErrCode Send(SOCKET &pSocket, const char *pData, int pSizeInBytes)
		{
			LOG("Net::Send()");
			int res;

			if (pSizeInBytes <= 0)
			{
				echo("ERROR: Can't send ", pSizeInBytes, " bytes.");
				return neterr_cantSendInvalidLength;
			}

			int offset = 0;
			while (true)
			{
				res = send(pSocket, &pData[offset], min(pSizeInBytes - offset, Net::GetBufferSize()), 0);
				if (res == SOCKET_ERROR)
				{
					echo("ERROR: Can't send data.");
					return neterr_cantSend;
				}
				else if (res == 0)
					return neterr_connectionClosed;

				offset += res;

				if (offset == pSizeInBytes)
				{
					// We sent all we want, so break the loop.
					// If not - continue send loop
					break;
				}
			}

			return neterr_noErr;
		}

	};

} // NetLib


#endif // NETLIB_NET_H
