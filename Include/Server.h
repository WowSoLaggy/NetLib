// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Product:		NetLib
// Module:		Server
// Author:		Anton Egorov
// Description:	Class that starts server on the given port,
//				accepts pending connections, sends and
//				receives data, and controls connected clients
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

#ifndef NETLIB_SERVER_H
#define NETLIB_SERVER_H


#include <mutex>
#include <string>
#include <functional>
#include <algorithm>

#include "NetErrCodes.h"
#include "Net.h"
#include "ClientData.h"


namespace NetLib
{

	// Typedef for the client accepted callback
	typedef std::function<void(CLIENTID pClientId, const std::string &pClientAddress, int pClientPort)> ClientAcceptCallback;

	// Typedef for the client disconnected callback
	typedef std::function<void(CLIENTID pClientId)> ClientDisconnectCallback;

	// Typedef for the data received callback
	typedef std::function<void(CLIENTID pClientId, char *pData, int pDataLength)> ReceiveFromClientCallback;


	// Class that starts server on the given port,
	// accepts pending connections, sends and
	// receives data, and controls connected clients
	class Server
	{
	public:

		// Creates a new server instance
		// Params:
		// [in] ClientAcceptCallback pOnClientAccepted			- callback that is called when the new client is accepted
		// [in] ClientDisconnectCallback pOnClientDisconnected	- callback that is called when the client is disconnected
		// [in] ReceiveFromClientCallback pOnClientDataReceived	- callback taht is called when the new data is received from the client
		Server(
			ClientAcceptCallback pOnClientAccepted,
			ClientDisconnectCallback pOnClientDisconnected,
			ReceiveFromClientCallback pOnReceiveFromClient)
		{
			m_onClientAccepted = pOnClientAccepted;
			m_onClientDisconnected = pOnClientDisconnected;
			m_onReceiveFromClient = pOnReceiveFromClient;
		}

		
		// Default virtual dtor
		virtual ~Server() { }



		// Returns whether the server is listening for the incoming connections
		bool IsListening()
		{
			return IsListening(m_sockListen);
		}


		// Checks whether the client with the given Id exists
		// Params:
		// [in] CLIENTID pClient	- Id of the client to check existance for
		bool CheckClientExists(CLIENTID pClientId)
		{
			m_clientsLock.lock();
			{
				auto it = std::find_if(m_clients.begin(), m_clients.end(), [&pClientId](const auto &client) { return client.Id == pClientId; });
				if (it == m_clients.end())
				{
					m_clientsLock.unlock();
					return false;
				}
			}
			m_clientsLock.unlock();
			return true;
		}



		// Starts the server on the given port
		// Params:
		// [in] int pPort	- port to start server on
		NetErrCode Start(int pPort)
		{
			LOG("Server::Start()");
			NetErrCode err;
			int res = 0;

			// Create socket

			err = Net::CreateSocket(m_sockListen);
			if (err != neterr_noErr)
			{
				echo("ERROR: Can't create socket to listen.");
				return err;
			}

			// Bind socket

			memset(&m_addrListen, 0, sizeof(m_addrListen));
			m_addrListen.sin_family = AF_INET;
			m_addrListen.sin_addr.s_addr = INADDR_ANY;
			m_addrListen.sin_port = htons(pPort);
			res = bind(m_sockListen, (sockaddr *)&m_addrListen, sizeof(sockaddr_in));
			if (res != 0)
			{
				echo("ERROR: Can't bind socket.");
				Net::CloseSocket(m_sockListen);
				return neterr_cantBindSocket;
			}

			// Set option SO_REUSEADDR

			err = Net::SetOptReuseAddr(m_sockListen);
			if (err != neterr_noErr)
			{
				echo("ERROR: Can't set socket SO_REUSEADDR option.");
				Net::CloseSocket(m_sockListen);
				return err;
			}

			// Set non-blocking mode

			err = Net::SetOptNonBlock(m_sockListen, true);
			if (err != neterr_noErr)
			{
				echo("ERROR: Can't set socket non-blocking mode.");
				Net::CloseSocket(m_sockListen);
				return err;
			}

			// Listen

			res = listen(m_sockListen, 5); // 5 is a magic number It should be OK
			if (res != 0)
			{
				echo("ERROR: Can't start listen socket.");
				Net::CloseSocket(m_sockListen);
				return neterr_cantStartListen;
			}

			// Start mainloop thread

			m_isRunning = true;
			m_receiveBuffer.resize(Net::GetBufferSize());
			HANDLE hThread = CreateThread(NULL, 0, MainLoopThreadStarter, this, CREATE_SUSPENDED, NULL);
			if (hThread == nullptr)
			{
				echo("ERROR: Can't create listen thread.");
				Net::CloseSocket(m_sockListen);
				return neterr_cantCreateThread;
			}
			ResumeThread(hThread);

			return neterr_noErr;
		}


		// Stops the server and disconnects all clients
		NetErrCode Stop()
		{
			m_runningLock.lock();
			m_isRunning = false;
			m_runningLock.unlock();
			return neterr_noErr;
		}



		// Sends the given data to the client with the given Id
		// Params:
		// [in] CLIENTID pClientId	- Id of the client to send data to
		// [in] const char * pData	- pointer to the data to send
		// [in] int pDataLength		- length of the data to send
		NetErrCode SendToClient(CLIENTID pClientId, const char *pData, int pDataLength)
		{
			LOG("Server::SendToClient()");
			NetErrCode err;
			bool res;

			res = CheckClientExists(pClientId);
			if (!res)
			{
				echo("ERROR: Can't find client to send data to.");
				return neterr_noSuchClient;
			}

			err = Net::Send(pClientId, pData, pDataLength);
			if (err != neterr_noErr)
			{
				echo("ERROR: Can't send data to client: ", pClientId);
				return err;
			}

			return neterr_noErr;
		}


		// Disconnects the client with the given Id
		// Params:
		// [in] CLIENTID pClientId	- Id of the client to be disconnected
		NetErrCode DisconnectClient(CLIENTID pClientId)
		{
			LOG("Server::DisconnectClient()");
			NetErrCode err;

			m_clientsLock.lock();
			{
				auto it = std::find_if(m_clients.begin(), m_clients.end(), [&pClientId](const auto &client) { return client.Id == pClientId; });
				if (it == m_clients.end())
				{
					m_clientsLock.unlock();
					echo("ERROR: Can't find client to disconnect: ", pClientId);
					return neterr_noSuchClient;
				}

				m_clients.erase(it);
				err = DisconnectClient_Internal(pClientId);
				if (err != neterr_noErr)
				{
					m_clientsLock.unlock();
					echo("ERROR: Can't disconnect client: ", pClientId);
					return neterr_errorDisconnecting;
				}
			}
			m_clientsLock.unlock();

			return neterr_noErr;
		}


	protected:


		volatile bool m_isRunning;							// Variable used to stop server
		std::mutex m_runningLock;							// Mutex to lock access to the m_isRunning variable. Used to stop server
		sockaddr_in m_addrListen;							// Address to listen on
		SOCKET m_sockListen;								// Socket that is used to listen for the pending connections
		ClientAcceptCallback m_onClientAccepted;			// Callback that is called when the new client is accepted
		ClientDisconnectCallback m_onClientDisconnected;	// Callback that is called when the client is disconnected
		ReceiveFromClientCallback m_onReceiveFromClient;	// Callback taht is called when the new data is received from the client

		std::mutex m_clientsLock;							// Mutex to lock access to the m_clients vector
		std::vector<ClientData> m_clients;					// Vector of connected clients
		std::vector<char> m_receiveBuffer;					// Buffer to receive data to


		// Returns whether the given socket is listening for the incoming connections
		bool IsListening(SOCKET pSocket)
		{
			int res;

			char val;
			int valLength = sizeof(val);
			res = getsockopt(pSocket, SOL_SOCKET, SO_ACCEPTCONN, &val, &valLength);
			return ((res != SOCKET_ERROR) && (val != 0));
		}


		// Disconnects the client with the given Id (no mutex lock)
		// Params:
		// [in] CLIENTID pClientId	- Id of the client to be disconnected
		NetErrCode DisconnectClient_Internal(CLIENTID pClientId)
		{
			LOG("Server::DisconnectClient_Internal()");
			NetErrCode err;

			err = Net::CloseSocket(pClientId);
			if (err != neterr_noErr)
			{
				echo("ERROR: Can't close socket.");
				return err;
			}

			if (m_onClientDisconnected != nullptr)
				m_onClientDisconnected(pClientId);

			return neterr_noErr;
		}



		// Static main loop starter
		// Params:
		// [in] LPVOID pParam	- pointer to the Server class instance
		static DWORD WINAPI MainLoopThreadStarter(LPVOID pParam)
		{
			if (pParam == nullptr)
				return -1;
			((Server *)pParam)->MainLoop();
			return 0;
		}



		// Main loop that repeats until the server is stopped
		void MainLoop()
		{
			LOG("Server::MainLoop()");
			NetErrCode err;

			while (true)
			{
				m_runningLock.lock();
				{
					if (!m_isRunning)
					{
						m_runningLock.unlock();
						break;
					}

					TryAccept();
					TryReceive();
					DisconnectClosed();
				}
				m_runningLock.unlock();

				Sleep(1);
			}

			err = Net::CloseSocket(m_sockListen);	// Stop listen
			if (err != neterr_noErr)
				echo("ERROR: Can't close listen socket.");

			err = Cleanup();
			if (err != neterr_noErr)
				echo("ERROR: Can't clean up.");
		}



		// Accepts pending connections if any and calls the OnClientAccepted callback
		void TryAccept()
		{
			sockaddr_in clientAddr;
			int clientAddrLength = sizeof(sockaddr_in);
			SOCKET sockClient;
			ClientData clientData;

			sockClient = accept(m_sockListen, (sockaddr *)&clientAddr, &clientAddrLength);
			if (sockClient != INVALID_SOCKET)
			{
				clientData = ClientData(sockClient, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

				m_clientsLock.lock();
				{
					// Add to m_clients only if it is not there already
					auto it = std::find_if(m_clients.begin(), m_clients.end(), [&sockClient](const auto &client) { return client.Id == sockClient; });
					if (it == m_clients.end())
						m_clients.push_back(clientData);
				}
				m_clientsLock.unlock();

				// Accepted someone
				if (m_onClientAccepted != nullptr)
					m_onClientAccepted(clientData.Id, clientData.Address, clientData.Port);
			}
		}


		// Checks all client's sockets if they have something to receive
		void TryReceive()
		{
			LOG("Server::TryReceive()");
			int res;

			m_clientsLock.lock();
			{
				if (m_clients.size() == 0)
				{
					// No one to receive from
					m_clientsLock.unlock();
					return;
				}

				fd_set fds;	// File descriptors set
				timeval timeout;
				timeout.tv_sec = 0;
				timeout.tv_usec = 0;

				// Reset the fd set
				FD_ZERO(&fds);

				// Add sockets to the fd set
				for (auto &client : m_clients)
					FD_SET(client.Id, &fds);

				// Check sockets
				res = select(0, &fds, nullptr, nullptr, &timeout);

				if (res == 0)
				{
					// No one is ready to receive
					m_clientsLock.unlock();
					return;
				}
				else if (res == SOCKET_ERROR)
				{
					echo("ERROR: Unknown error occurred while selecting sockets.");
					m_clientsLock.unlock();
					return;
				}

				// Check who is ready to receive
				for (auto &client : m_clients)
				{
					if (!FD_ISSET(client.Id, &fds))
						continue;

					int bytesReceived = recv(client.Id, m_receiveBuffer.data(), Net::GetBufferSize(), 0);

					if (bytesReceived == SOCKET_ERROR)
					{
						// Connection error or forced hard-close
						echo("ERROR: Can't receive data. Disconnect client: ", client.Id);
						client.Closed = true;
						continue;
					}
					else if (bytesReceived == 0)
					{
						// Connection has been gracefully closed. Not an error
						client.Closed = true;
						continue;
					}

					if (m_onReceiveFromClient != nullptr)
						m_onReceiveFromClient(client.Id, m_receiveBuffer.data(), bytesReceived);
				}
			}
			m_clientsLock.unlock();
		}



		// Disconnects clients with the closed connection
		void DisconnectClosed()
		{
			LOG("Server::DisconnectClosed()");
			NetErrCode err;

			m_clientsLock.lock();
			{
				if (m_clients.size() == 0)
				{
					// No one to disconnect
					m_clientsLock.unlock();
					return;
				}

				for (int i = (int)(m_clients.size() - 1); i >= 0; i--)
				{
					if (!m_clients[i].Closed)
						continue;

					CLIENTID clientId = m_clients[i].Id;
					m_clients.erase(m_clients.begin() + i);
					err = DisconnectClient_Internal(clientId);
					if (err != neterr_noErr)
					{
						// No return, continue to kick closed connections
						echo("ERROR: Can't disconnect client: ", clientId);
					}
				}
			}
			m_clientsLock.unlock();
		}



		// Disconnects all clients and releases used memory
		NetErrCode Cleanup()
		{
			LOG("Server::Cleanup()");
			NetErrCode err;

			m_clientsLock.lock();
			{
				for (auto &client : m_clients)
				{
					err = Net::CloseSocket(client.Id);
					if (err != neterr_noErr)
					{
						// No return - continue to dispose all clients
						echo("ERROR: Can't close socket: ", client.Id);
					}
				}

				std::vector<ClientData>().swap(m_clients);
			}
			m_clientsLock.unlock();

			std::vector<char>().swap(m_receiveBuffer);

			return neterr_noErr;
		}

	};

} // NetLib


#endif // NETLIB_SERVER_H
