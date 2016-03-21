#include "stdafx.h"
#include "Server.h"


namespace NetLib
{

	Server::Server() { }
	Server::~Server() { }


	NetErrCode Server::Start(int pPort, std::function<void(std::string pClientAddress, int pClientPort)> pOnClientAccepted)
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

		// Start listen thread

		m_isListen = true;
		HANDLE hThread = CreateThread(NULL, 0, ListenThreadStarter, this, CREATE_SUSPENDED, NULL);
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
		m_isListen = false;
		return neterr_noErr;
	}

	

	bool Server::CheckIsListening(SOCKET pSocket)
	{
		int res;

		char val;
		int valLength = sizeof(val);
		res = getsockopt(pSocket, SOL_SOCKET, SO_ACCEPTCONN, &val, &valLength);
		return ((res != SOCKET_ERROR) && (val != 0));
	}

	DWORD WINAPI Server::ListenThreadStarter(LPVOID pParam)
	{
		if (pParam == nullptr)
			return -1;
		((Server *)pParam)->ListenLoop();
		return 0;
	}

	void Server::ListenLoop()
	{
		sockaddr_in clientAddr;
		int clientAddrLength = sizeof(sockaddr_in);
		SOCKET sockClient;

		while (m_isListen)
		{
			sockClient = accept(m_sockListen, (sockaddr *)&clientAddr, &clientAddrLength);
			if (sockClient != INVALID_SOCKET)
			{
				// Accepted someone
				if (m_onClientAccepted != nullptr)
					m_onClientAccepted(inet_ntoa(clientAddr.sin_addr), clientAddr.sin_port);
			}

			Sleep(1);
		}

		// Stop listen

		CloseSocket(m_sockListen);
	}

	bool Server::IsListening()
	{
		return CheckIsListening(m_sockListen);
	}

} // NetLib
