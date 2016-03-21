#include "stdafx.h"
#include "Server.h"


namespace NetLib
{

	Server::Server() { }
	Server::~Server() { }


	NetErrCode Server::Start(int pPort, AcceptCallback pOnClientAccepted)
	{
		LOG("Server::Start()");
		NetErrCode err;
		int res = 0;

		m_onClientAccepted = pOnClientAccepted;

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

		err = Net::SetOptNonBlock(m_sockListen);
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
			CloseSocket(m_sockListen);
			return neterr_cantBindSocket;
		}

		// Listen

		res = listen(m_sockListen, 5); // 5 is a magic number It should be OK
		if (res != 0)
		{
			echo("Can't start listen socket.");
			CloseSocket(m_sockListen);
			return neterr_cantStartListen;
		}

		// Start mainloop thread

		m_isRunning = true;
		m_receiveBuffer.resize(Net::GetBufferSize());
		HANDLE hThread = CreateThread(NULL, 0, MainLoopThreadStarter, this, CREATE_SUSPENDED, NULL);
		if (hThread == nullptr)
		{
			echo("Can't create listen thread.");
			CloseSocket(m_sockListen);
			return neterr_cantCreateThread;
		}
		ResumeThread(hThread);

		return neterr_noErr;
	}

	NetErrCode Server::Stop()
	{
		m_isRunning = false;
		return neterr_noErr;
	}

	NetErrCode Server::DisconnectClient(unsigned int pClientId)
	{
		LOG("Server::DisconnectClient()");
		NetErrCode err;

		m_clientsLock.lock();
		{
			auto it = std::find(m_clients.begin(), m_clients.end(), pClientId);
			if (it == m_clients.end())
			{
				m_clientsLock.unlock();
				echo("Can't find the given client to disconnect.");
				return neterr_noSuchClient;
			}

			m_clients.erase(it);
		}
		m_clientsLock.unlock();

		err = CloseSocket(pClientId);
		if (err != neterr_noErr)
		{
			echo("Can't close socket.");
			return err;
		}

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

		while (m_isRunning)
		{
			TryAccept();
			TryReceive();

			Sleep(1);
		}

		err = CloseSocket(m_sockListen);	// Stop listen
		if (err != neterr_noErr)
			echo("Can't close listen socket.");

		err = Cleanup();
		if (err != neterr_noErr)
			echo("Error while cleaning up.");
	}

	NetErrCode Server::Cleanup()
	{
		LOG("Server::Cleanup()");
		NetErrCode err;

		m_clientsLock.lock();
		{
			for (auto &sock : m_clients)
			{
				err = CloseSocket(sock);
				if (err != neterr_noErr)
				{
					// No return - continue to dispose all clients
					echo("Can't close socket: ", sock);
				}
			}

			std::vector<SOCKET>().swap(m_clients);
		}
		m_clientsLock.unlock();

		std::vector<char>().swap(m_receiveBuffer);

		return neterr_noErr;
	}

} // NetLib
