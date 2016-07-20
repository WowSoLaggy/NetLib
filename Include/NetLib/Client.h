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


#include <string>

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
		Client() { }

		// Default dtor
		virtual ~Client() { }


		// Connects to the remote server with the given address
		// Params:
		// [in] const std::string & pServerIp	- string that represents the remote server address
		// [in] int pServerPort					- remote server port
		NetErrCode Connect(const std::string &pServerIp, int pServerPort)
		{
			LOG("Client::Connect()");
			NetErrCode err;
			int res;

			// Create socket

			err = Net::CreateSocket(m_sockClient);
			if (err != neterr_noErr)
			{
				echo("ERROR: Can't create socket to listen.");
				return err;
			}

			// Set option SO_REUSEADDR

			err = Net::SetOptReuseAddr(m_sockClient);
			if (err != neterr_noErr)
			{
				echo("ERROR: Can't set socket SO_REUSEADDR option.");
				Net::CloseSocket(m_sockClient);
				return err;
			}

			// Connect to server

			sockaddr_in addr;
			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = inet_addr(pServerIp.c_str());
			addr.sin_port = htons(pServerPort);
			res = connect(m_sockClient, (const sockaddr *)&addr, sizeof(sockaddr_in));
			if (res != 0)
			{
				res = WSAGetLastError();
				if (res == WSAECONNREFUSED)
				{
					// Connection refused.
					// No connection could be made because the target computer actively refused it.
					// This usually results from trying to connect to a service that is inactive
					// on the foreign host—that is, one with no server application running.
					echo("ERROR: Server (", pServerIp, ":", pServerPort, ") refused connection.");
					return neterr_connectionRefused;
				}
				else if (res == WSAETIMEDOUT)
				{
					// Connection timed out.
					// A connection attempt failed because the connected party did not properly respond after a period
					// of time, or the established connection failed because the connected host has failed to respond.
					echo("ERROR: Connection to ", pServerIp, ":", pServerPort, " timed out.");
					return neterr_connectionTimedOut;
				}

				// Unknown error
				echo("ERROR: Can't connect to the server: ", pServerIp, ":", pServerPort);
				Net::CloseSocket(m_sockClient);
				return neterr_cantConnect;
			}

			return neterr_noErr;
		}


		// Disconnects from the remote server
		NetErrCode Disconnect()
		{
			LOG("Client::Disconnect()");
			NetErrCode err;

			err = Net::CloseSocket(m_sockClient);
			if (err != neterr_noErr)
			{
				echo("ERROR: Can't close client socket.");
				return err;
			}

			return neterr_noErr;
		}

		

		// Sends the given data to the remote server
		// Params:
		// [in] const char * pData	- data to send to the remote server
		// [in] int pSizeInBytes	- size of the data to be sent
		NetErrCode Send(const char *pData, int pSizeInBytes)
		{
			LOG("Client::Send()");
			NetErrCode err;

			err = Net::Send(m_sockClient, pData, pSizeInBytes);
			if (err != neterr_noErr)
			{
				echo("ERROR: Can't send data to client.");
				return err;
			}

			return neterr_noErr;
		}


		// Tries to receive data from the remote server if any
		// Params:
		// [in]  char * pBuffer				- buffer to receive data to
		// [in]  int pBufferSize			- length of the buffer to receive data to
		// [out] int & pReceivedDataLength	- length of the received data (can be 0 if nothing to receive)
		NetErrCode Receive(char *pBuffer, int pBufferSize, int &pReceivedDataLength)
		{
			LOG("Client::Receive()");
			int res;

			pReceivedDataLength = 0;

			fd_set fds;	// File descriptors set
			timeval timeout;
			timeout.tv_sec = 0;
			timeout.tv_usec = 0;

			// Reset the fd set
			FD_ZERO(&fds);

			// Add socket to the fd set
			FD_SET(m_sockClient, &fds);

			// Check sockets
			res = select(0, &fds, nullptr, nullptr, &timeout);

			if (res == 0)
			{
				// Nothing to receive
				return neterr_noErr;
			}
			else if (res == SOCKET_ERROR)
			{
				echo("ERROR: Unknown error occurred while selecting socket.");
				return neterr_cantSelect;
			}

			if (!FD_ISSET(m_sockClient, &fds))
			{
				// Strange but OK
				return neterr_noErr;
			}

			int bytesReceived = recv(m_sockClient, pBuffer, pBufferSize, 0);

			if (bytesReceived == SOCKET_ERROR)
			{
				echo("ERROR: Can't receive data.");
				return neterr_cantReceive;
			}
			else if (bytesReceived == 0)
			{
				// Connection has been gracefully closed. Not an error
				return neterr_connectionClosed;
			}

			pReceivedDataLength = bytesReceived;

			return neterr_noErr;
		}


	protected:


		SOCKET m_sockClient;	// Socket to connect to the remote server
	};

} // NetLib


#endif // NETLIB_CLIENT_H
