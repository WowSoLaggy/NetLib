#include "stdafx.h"
#include "Client.h"


namespace NetLib
{

	Client::Client() { }
	Client::~Client() { }

	
	NetErrCode Client::Connect(std::string pServerIp, int pServerPort)
	{
		LOG("Client::Connect()");
		NetErrCode err;
		int res;

		// Create socket

		err = Net::CreateSocket(m_sockClient);
		if (err != neterr_noErr)
		{
			echo("Can't create socket to listen.");
			return err;
		}

		// Set option SO_REUSEADDR

		err = Net::SetOptReuseAddr(m_sockClient);
		if (err != neterr_noErr)
		{
			echo("Can't set socket SO_REUSEADDR option.");
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
				echo("Server (", pServerIp, ":", pServerPort, ") refused connection.");
				return neterr_connectionRefused;
			}
			else if (res == WSAETIMEDOUT)
			{
				// Connection timed out.
				// A connection attempt failed because the connected party did not properly respond after a period
				// of time, or the established connection failed because the connected host has failed to respond.
				echo("Connection to ", pServerIp, ":", pServerPort, " timed out.");
				return neterr_connectionTimedOut;
			}

			// Unknown error
			echo("Can't connect to the server: ", pServerIp, ":", pServerPort);
			Net::CloseSocket(m_sockClient);
			return neterr_cantConnect;
		}

		return neterr_noErr;
	}

	NetErrCode Client::Disconnect()
	{
		LOG("Client::Disconnect()");
		NetErrCode err;

		err = Net::CloseSocket(m_sockClient);
		if (err != neterr_noErr)
		{
			echo("Can't close client socket.");
			return err;
		}

		return neterr_noErr;
	}

	NetErrCode Client::Send(const char *pData, int pSizeInBytes)
	{
		LOG("Client::Send()");
		NetErrCode err;

		err = Net::Send(m_sockClient, pData, pSizeInBytes);
		if (err != neterr_noErr)
		{
			echo("Can't send data to client.");
			return err;
		}

		return neterr_noErr;
	}

	NetErrCode Client::Receive(char *pBuffer, int pBufferSize, int &pReceivedDataLength)
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
			echo("Unknown error occurred while selecting socket.");
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
			echo("Error receiving data.");
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

} // NetLib
