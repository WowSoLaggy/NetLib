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
		Client(ClientCb_ReceivedFromServer pOnReceivedFromServer)
			: m_onReceivedFromServer(pOnReceivedFromServer)
		{
		}


		//
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

			// Start mainloop thread

			m_isRunning = true;
			m_receiveBuffer.resize(Net::GetBufferSize());
			std::thread mainThread(&Client::MainLoop, this);
			mainThread.detach();

			return neterr_noErr;
		}


		// Disconnects from the remote server
		NetErrCode Disconnect()
		{
			m_isRunning = false;
			return neterr_noErr;
		}

		

		// Sends the given data to the remote server
		// Params:
		// [in] const char * pData	- data to send to the remote server
		// [in] int pSizeInBytes	- size of the data to be sent
		NetErrCode SendToServer(const char *pData, int pSizeInBytes)
		{
			LOG("Client::SendToServer()");
			NetErrCode err;

			err = Net::Send(m_sockClient, pData, pSizeInBytes);
			if (err != neterr_noErr)
			{
				echo("ERROR: Can't send data to client.");
				return err;
			}

			return neterr_noErr;
		}



	protected:


		std::atomic_bool m_isRunning;						// Variable used to stop the client
		SOCKET m_sockClient;								// Socket to connect to the remote server
		std::vector<char> m_receiveBuffer;					// Buffer to receive data to

		ClientCb_ReceivedFromServer m_onReceivedFromServer;	// Callback that is called when the new data is received from the server



		// Main loop that repeats until the client is stopped
		void MainLoop()
		{
			LOG("Client::MainLoop()");
			NetErrCode err;

			while (true)
			{
				if (!m_isRunning)
					break;

				TryReceive();

				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}

			err = Net::CloseSocket(m_sockClient);
			if (err != neterr_noErr)
				echo("ERROR: Can't close client socket.");
		}



		// Tries to receive data from the remote server if any
		void TryReceive()
		{
			LOG("Client::TryReceive()");
			NetErrCode err;
			int res;

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
				return;
			}
			else if (res == SOCKET_ERROR)
			{
				echo("ERROR: Unknown error occurred while selecting socket.");
				return;
			}

			if (!FD_ISSET(m_sockClient, &fds))
			{
				// Strange but OK
				return;
			}

			int bytesReceived = recv(m_sockClient, m_receiveBuffer.data(), Net::GetBufferSize(), 0);
			if (bytesReceived > 0)
			{
				if (m_onReceivedFromServer != nullptr)
					m_onReceivedFromServer(m_receiveBuffer.data(), bytesReceived);

				return;
			}

			if (bytesReceived == SOCKET_ERROR)
			{
				// Connection error or forced hard-close. Disconnect
				echo("ERROR: Error receiving data. Force disconnect.");
			}
			else if (bytesReceived == 0)
			{
				// Connection has been gracefully closed. Not an error
			}

			err = Disconnect();
			if (err != neterr_noErr)
				echo("ERROR: Can't disconnect.");
		}
	};

} // NetLib


#endif // NETLIB_CLIENT_H
