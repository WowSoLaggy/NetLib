#pragma once

#ifndef SERVER_H
#define SERVER_H


#include "Logger.h"
#include "NetErrCodes.h"
#include "Net.h"


namespace NetLib
{
	class Server : public Net
	{
	public:

		Server();
		virtual ~Server();

		bool IsListening();

		NetErrCode Start(int pPort, std::function<void(std::string pClientAddress, int pClientPort)> pOnClientAccepted);
		NetErrCode Stop();

	protected:

		volatile bool m_isListen;
		sockaddr_in m_addrListen;
		SOCKET m_sockListen;

		std::function<void(std::string pClientAddress, int pClientPort)> m_onClientAccepted;

		bool CheckIsListening(SOCKET pSocket);

		static DWORD WINAPI ListenThreadStarter(LPVOID pParam);
		void ListenLoop();
	};

} // NetLib


#endif // SERVER_H
