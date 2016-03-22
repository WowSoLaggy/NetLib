#pragma once

#ifndef SERVER_H
#define SERVER_H


#include "Logger.h"
#include "NetErrCodes.h"
#include "Net.h"
#include "ClientData.h"


namespace NetLib
{
	typedef std::function<void(unsigned int pClientId, std::string pClientAddress, int pClientPort)> AcceptCallback;
	typedef std::function<void(unsigned int pClientId)> DisconnectCallback;
	typedef std::function<void(unsigned int pClientId, char *pData, int pDataLength)> ReceiveCallback;

	class Server : public Net
	{
	public:

		Server();
		virtual ~Server();

		bool IsListening();

		NetErrCode Start(int pPort, AcceptCallback pOnClientAccepted, DisconnectCallback pOnClientDisconnected, ReceiveCallback pOnClientDataReceived);
		NetErrCode Stop();

		NetErrCode DisconnectClient(unsigned int pClientId);

	protected:

		volatile bool m_isRunning;
		std::mutex m_runningLock;
		sockaddr_in m_addrListen;
		SOCKET m_sockListen;
		AcceptCallback m_onClientAccepted;
		DisconnectCallback m_onClientDisconnected;
		ReceiveCallback m_onClientDataReceived;

		std::mutex m_clientsLock;
		std::vector<ClientData> m_clients;
		std::vector<char> m_receiveBuffer;

		bool CheckIsListening(SOCKET pSocket);
		NetErrCode DisconnectClient_Internal(unsigned int pClientId);

		static DWORD WINAPI MainLoopThreadStarter(LPVOID pParam);
		void MainLoop();
		void TryAccept();
		void TryReceive();
		void DisconnectClosed();

		NetErrCode Cleanup();
	};

} // NetLib


#endif // SERVER_H
