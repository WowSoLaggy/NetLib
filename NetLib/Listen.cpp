#include "stdafx.h"
#include "Server.h"


namespace NetLib
{

	bool Server::CheckIsListening(SOCKET pSocket)
	{
		int res;

		char val;
		int valLength = sizeof(val);
		res = getsockopt(pSocket, SOL_SOCKET, SO_ACCEPTCONN, &val, &valLength);
		return ((res != SOCKET_ERROR) && (val != 0));
	}

	bool Server::IsListening()
	{
		return CheckIsListening(m_sockListen);
	}

	void Server::TryAccept()
	{
		sockaddr_in clientAddr;
		int clientAddrLength = sizeof(sockaddr_in);
		SOCKET sockClient;

		sockClient = accept(m_sockListen, (sockaddr *)&clientAddr, &clientAddrLength);
		if (sockClient != INVALID_SOCKET)
		{
			m_clientsLock.lock();
			{
				// Add to m_clients only if it is not there already
				auto it = std::find(m_clients.begin(), m_clients.end(), sockClient);
				if (it == m_clients.end())
					m_clients.push_back(sockClient);
			}
			m_clientsLock.unlock();

			// Accepted someone
			if (m_onClientAccepted != nullptr)
				m_onClientAccepted(sockClient, inet_ntoa(clientAddr.sin_addr), clientAddr.sin_port);
		}
	}

} // NetLib
