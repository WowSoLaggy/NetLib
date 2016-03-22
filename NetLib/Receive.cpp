#include "stdafx.h"
#include "Server.h"


namespace NetLib
{

	void Server::TryReceive()
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
				FD_SET(client.Sock, &fds);

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
				echo("Unknown error occurred while selecting sockets.");
				m_clientsLock.unlock();
				return;
			}

			// Check who is ready to receive
			for (auto &client : m_clients)
			{
				if (!FD_ISSET(client.Sock, &fds))
					continue;

				int bytesReceived = recv(client.Sock, m_receiveBuffer.data(), Net::GetBufferSize(), 0);

				if (bytesReceived == SOCKET_ERROR)
				{
					echo("Error receiving data. Disconnect client: ", client.Sock);
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
					m_onReceiveFromClient(client.Sock, m_receiveBuffer.data(), bytesReceived);
			}
		}
		m_clientsLock.unlock();
	}

} // NetLib
