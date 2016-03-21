#pragma once

#ifndef SERVER_H
#define SERVER_H


#include "Logger.h"
#include "NetErrCodes.h"
#include "Net.h"


namespace NetLib
{
	typedef std::function<void(unsigned int pClientId, std::string pClientAddress, int pClientPort)> AcceptCallback;
	typedef std::function<void(unsigned int pClientId, char *pData)> ReceiveCallback;

	class Server : public Net
	{
	public:

		Server();
		virtual ~Server();

		bool IsListening();

		NetErrCode Start(int pPort, AcceptCallback pOnClientAccepted);
		NetErrCode Stop();

		NetErrCode DisconnectClient(unsigned int pClientId);

	protected:

		volatile bool m_isRunning;
		sockaddr_in m_addrListen;
		SOCKET m_sockListen;
		AcceptCallback m_onClientAccepted;

		std::mutex m_clientsLock;
		std::vector<SOCKET> m_clients;
		std::vector<char> m_receiveBuffer;

		bool CheckIsListening(SOCKET pSocket);

		static DWORD WINAPI MainLoopThreadStarter(LPVOID pParam);
		void MainLoop();
		void TryAccept();
		void TryReceive();

		NetErrCode Cleanup();
	};

} // NetLib


#endif // SERVER_H
