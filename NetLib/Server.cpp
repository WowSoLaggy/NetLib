#include "stdafx.h"
#include "Server.h"


namespace NetLib
{

	Server::Server(
		ClientAcceptCallback pOnClientAccepted,
		ClientDisconnectCallback pOnClientDisconnected,
		ReceiveFromClientCallback pOnReceiveFromClient)
	{
		m_onClientAccepted = pOnClientAccepted;
		m_onClientDisconnected = pOnClientDisconnected;
		m_onReceiveFromClient = pOnReceiveFromClient;
	}

	Server::~Server() { }


	NetErrCode Server::Start(int pPort)
	{
		LOG("Server::Start()");
		NetErrCode err;
		int res = 0;

		// Create socket

		err = Net::CreateSocket(m_sockListen);
		if (err != neterr_noErr)
		{
			echo("Can't create socket to listen.");
			return err;
		}

		// Set option SO_REUSEADDR

		err = Net::SetOptReuseAddr(m_sockListen);
		if (err != neterr_noErr)
		{
			echo("Can't set socket SO_REUSEADDR option.");
			Net::CloseSocket(m_sockListen);
			return err;
		}

		// Set non-blocking mode

		err = Net::SetOptNonBlock(m_sockListen, true);
		if (err != neterr_noErr)
		{
			echo("Can't set socket non-blocking mode.");
			Net::CloseSocket(m_sockListen);
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
			echo("Can't bind socket.");
			Net::CloseSocket(m_sockListen);
			return neterr_cantBindSocket;
		}

		// Listen

		res = listen(m_sockListen, 5); // 5 is a magic number It should be OK
		if (res != 0)
		{
			echo("Can't start listen socket.");
			Net::CloseSocket(m_sockListen);
			return neterr_cantStartListen;
		}

		// Start mainloop thread

		m_isRunning = true;
		m_receiveBuffer.resize(Net::GetBufferSize());
		HANDLE hThread = CreateThread(NULL, 0, MainLoopThreadStarter, this, CREATE_SUSPENDED, NULL);
		if (hThread == nullptr)
		{
			echo("Can't create listen thread.");
			Net::CloseSocket(m_sockListen);
			return neterr_cantCreateThread;
		}
		ResumeThread(hThread);

		return neterr_noErr;
	}

	NetErrCode Server::Stop()
	{
		m_runningLock.lock();
		m_isRunning = false;
		m_runningLock.unlock();
		return neterr_noErr;
	}

	NetErrCode Server::SendToClient(unsigned int pClientId, const char *pData, int pDataLength)
	{
		LOG("Server::SendToClient()");
		NetErrCode err;

		err = Net::Send(pClientId, pData, pDataLength);
		if (err != neterr_noErr)
		{
			echo("Error sending data to client: ", pClientId);
			return err;
		}

		return neterr_noErr;
	}

	NetErrCode Server::DisconnectClient(unsigned int pClientId)
	{
		LOG("Server::DisconnectClient()");
		NetErrCode err;

		m_clientsLock.lock();
		{
			auto it = std::find_if(m_clients.begin(), m_clients.end(), [&pClientId](const auto &client) { return client.Sock == pClientId; });
			if (it == m_clients.end())
			{
				m_clientsLock.unlock();
				echo("Can't find the given client to disconnect: ", pClientId);
				return neterr_noSuchClient;
			}

			m_clients.erase(it);
			err = DisconnectClient_Internal(pClientId);
			if (err != neterr_noErr)
			{
				m_clientsLock.unlock();
				echo("Error while disconnecting client: ", pClientId);
				return neterr_errorDisconnecting;
			}
		}
		m_clientsLock.unlock();

		return neterr_noErr;
	}

	NetErrCode Server::DisconnectClient_Internal(unsigned int pClientId)
	{
		LOG("Server::DisconnectClient_Internal()");
		NetErrCode err;

		err = Net::CloseSocket(pClientId);
		if (err != neterr_noErr)
		{
			echo("Can't close socket.");
			return err;
		}

		if (m_onClientDisconnected != nullptr)
			m_onClientDisconnected(pClientId);

		return neterr_noErr;
	}


	DWORD WINAPI Server::MainLoopThreadStarter(LPVOID pParam)
	{
		if (pParam == nullptr)
			return -1;
		((Server *)pParam)->MainLoop();
		return 0;
	}

	void Server::MainLoop()
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
			echo("Can't close listen socket.");

		err = Cleanup();
		if (err != neterr_noErr)
			echo("Error while cleaning up.");
	}

	void Server::DisconnectClosed()
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

				unsigned int clientId = m_clients[i].Sock;
				m_clients.erase(m_clients.begin() + i);
				err = DisconnectClient_Internal(clientId);
				if (err != neterr_noErr)
				{
					// No return, continue to kick closed connections
					echo("Error while disconnecting client: ", clientId);
				}
			}
		}
		m_clientsLock.unlock();
	}

	NetErrCode Server::Cleanup()
	{
		LOG("Server::Cleanup()");
		NetErrCode err;

		m_clientsLock.lock();
		{
			for (auto &client : m_clients)
			{
				err = Net::CloseSocket(client.Sock);
				if (err != neterr_noErr)
				{
					// No return - continue to dispose all clients
					echo("Can't close socket: ", client.Sock);
				}
			}

			std::vector<ClientData>().swap(m_clients);
		}
		m_clientsLock.unlock();

		std::vector<char>().swap(m_receiveBuffer);

		return neterr_noErr;
	}

} // NetLib
