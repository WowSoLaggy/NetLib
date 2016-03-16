#include "stdafx.h"
#include "Server.h"


#pragma comment(lib, "Ws2_32.lib")	// Link socket library


#define SD_BOTH	0x02				// Manifest constant for shutdown() 


namespace NetLib
{

	Server::Server() { }
	Server::~Server() { }


	NetErrCode Server::Start(int pPort, std::function<void(std::string pClientAddress, int pClientPort)> pOnClientAccepted)
	{
		LOG("Server::Start()");
		int res = 0;

		m_onClientAccepted = pOnClientAccepted;

		// Initialize socket library

		WSADATA wsaData;
		res = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (res != 0)
		{
			echo("Can't initialize socket library.");
			return neterr_noSocketLibrary;
		}

		// Create socket

		m_sockListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_sockListen == INVALID_SOCKET)
		{
			echo("Can't create socket to listen.");
			return neterr_cantCreateSocket;
		}

		// Set option SO_REUSEADDR

		int int_on = 1;
		res = setsockopt(m_sockListen, SOL_SOCKET, SO_REUSEADDR, (const char *)&int_on, sizeof(int_on));
		if (res != 0)
		{
			echo("Can't set socket SO_REUSEADDR option.");
			CloseSocket(m_sockListen);
			return neterr_cantSetSocketOptions;
		}

		// Set non-blocking mode

		u_long u_long_on = 1;
		res = ioctlsocket(m_sockListen, FIONBIO, &u_long_on);
		if (res != 0)
		{
			echo("Can't set socket non-blocking mode.");
			CloseSocket(m_sockListen);
			return neterr_cantSetSocketOptions;
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

	NetErrCode Server::CloseSocket(SOCKET pSocket)
	{
		shutdown(pSocket, SD_BOTH);
		closesocket(pSocket);

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
			}

			Sleep(1);
		}

		// Stop listen

		CloseSocket(m_sockListen);
	}

} // NetLib
