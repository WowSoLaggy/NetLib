#pragma once

#ifndef SERVER_H
#define SERVER_H


#include "Logger.h"
#include "NetErrCodes.h"


namespace NetLib
{

	class Server
	{
	public:

		Server();
		virtual ~Server();

		NetErrCode Start(int pPort, std::function<void(std::string pClientAddress, int pClientPort)> pOnClientAccepted);
		NetErrCode Stop();

	private:

		volatile bool m_isListen;
		SOCKET m_sockListen;
		sockaddr_in m_addrListen;

		std::function<void(std::string pClientAddress, int pClientPort)> m_onClientAccepted;

		NetErrCode CloseSocket(SOCKET pSocket);
		bool CheckIsListening(SOCKET pSocket);

		static DWORD WINAPI ListenThreadStarter(LPVOID pParam);
		void ListenLoop();
	};

} // NetLib


#endif // SERVER_H
