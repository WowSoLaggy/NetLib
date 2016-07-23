#include "Server.h"

#include <algorithm>

#include "Log.h"
#include "Net.h"


namespace NetLib
{

	Server::Server(
		ServerCb_ClientAccepted pOnClientAccepted,
		ServerCb_ClientDiconnected pOnClientDisconnected,
		ServerCb_ReceivedFromClient pOnReceivedFromClient)
		: m_onClientAccepted(pOnClientAccepted), m_onClientDisconnected(pOnClientDisconnected), m_onReceivedFromClient(pOnReceivedFromClient)
	{
	}

	Server::~Server() { }


	bool Server::IsListening() const
	{
		return IsListening(m_sockListen);
	}

	bool Server::CheckClientExists(CLIENTID pClientId) const
	{
		std::unique_lock<std::recursive_mutex> lock(m_clientsLock);

		auto it = std::find_if(m_clients.begin(), m_clients.end(), [&pClientId](const auto &client) { return client.Id == pClientId; });
		if (it == m_clients.end())
			return false;

		return true;
	}

	
	NetErrCode Server::Start(int pPort)
	{
		LOG("Server::Start()");
		NetErrCode err;
		int res = 0;

		// Create socket

		err = Net::CreateSocket(m_sockListen);
		if (err != neterr_noErr)
		{
			echo("ERROR: Can't create socket to listen.");
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
			echo("ERROR: Can't bind socket.");
			Net::CloseSocket(m_sockListen);
			return neterr_cantBindSocket;
		}

		// Set option SO_REUSEADDR

		err = Net::SetOptReuseAddr(m_sockListen);
		if (err != neterr_noErr)
		{
			echo("ERROR: Can't set socket SO_REUSEADDR option.");
			Net::CloseSocket(m_sockListen);
			return err;
		}

		// Set non-blocking mode

		err = Net::SetOptNonBlock(m_sockListen, true);
		if (err != neterr_noErr)
		{
			echo("ERROR: Can't set socket non-blocking mode.");
			Net::CloseSocket(m_sockListen);
			return err;
		}

		// Listen

		res = listen(m_sockListen, 5); // 5 is a magic number It should be OK
		if (res != 0)
		{
			echo("ERROR: Can't start listen socket.");
			Net::CloseSocket(m_sockListen);
			return neterr_cantStartListen;
		}

		// Start mainloop thread

		m_isRunning = true;
		m_receiveBuffer.resize(Net::GetBufferSize());
		std::thread mainThread(&Server::MainLoop, this);
		mainThread.detach();

		return neterr_noErr;
	}

	NetErrCode Server::Stop()
	{
		m_isRunning = false;
		return neterr_noErr;
	}


	NetErrCode Server::SendToClient(CLIENTID pClientId, const char *pData, int pDataLength)
	{
		LOG("Server::SendToClient()");
		NetErrCode err;
		bool res;

		res = CheckClientExists(pClientId);
		if (!res)
		{
			echo("ERROR: Can't find client to send data to.");
			return neterr_noSuchClient;
		}

		err = Net::Send(pClientId, pData, pDataLength);
		if (err != neterr_noErr)
		{
			echo("ERROR: Can't send data to client: ", pClientId, ".");
			return err;
		}

		return neterr_noErr;
	}


	NetErrCode Server::DisconnectClient(CLIENTID pClientId)
	{
		LOG("Server::DisconnectClient()");
		NetErrCode err;

		std::unique_lock<std::recursive_mutex> lock(m_clientsLock);

		auto it = std::find_if(m_clients.begin(), m_clients.end(), [&pClientId](const auto &client) { return client.Id == pClientId; });
		if (it == m_clients.end())
		{
			echo("ERROR: Can't find client to disconnect: ", pClientId, ".");
			return neterr_noSuchClient;
		}

		m_clients.erase(it);
		err = DisconnectClient_internal(pClientId);
		if (err != neterr_noErr)
		{
			echo("ERROR: Can't disconnect client: ", pClientId, ".");
			return neterr_errorDisconnecting;
		}

		return neterr_noErr;
	}


	bool Server::IsListening(SOCKET pSocket) const
	{
		int res;

		char val;
		int valLength = sizeof(val);
		res = getsockopt(pSocket, SOL_SOCKET, SO_ACCEPTCONN, &val, &valLength);
		return ((res != SOCKET_ERROR) && (val != 0));
	}


	NetErrCode Server::DisconnectClient_internal(CLIENTID pClientId)
	{
		LOG("Server::DisconnectClient_internal()");
		NetErrCode err;

		err = Net::CloseSocket(pClientId);
		if (err != neterr_noErr)
		{
			echo("ERROR: Can't close socket.");
			return err;
		}

		if (m_onClientDisconnected != nullptr)
			m_onClientDisconnected(pClientId);

		return neterr_noErr;
	}


	void Server::MainLoop()
	{
		LOG("Server::MainLoop()");
		NetErrCode err;

		while (true)
		{
			if (!m_isRunning)
				break;

			TryAccept();
			TryReceive();

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		err = Net::CloseSocket(m_sockListen);	// Stop listen
		if (err != neterr_noErr)
			echo("ERROR: Can't close listen socket.");

		err = Cleanup();
		if (err != neterr_noErr)
			echo("ERROR: Can't clean up.");
	}

	void Server::TryAccept()
	{
		sockaddr_in clientAddr;
		int clientAddrLength = sizeof(sockaddr_in);
		SOCKET sockClient;
		ClientData clientData;

		sockClient = accept(m_sockListen, (sockaddr *)&clientAddr, &clientAddrLength);
		if (sockClient != INVALID_SOCKET)
		{
			clientData = ClientData(sockClient, inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

			{
				std::unique_lock<std::recursive_mutex> lock(m_clientsLock);

				// Add to m_clients only if it is not there already
				auto it = std::find_if(m_clients.begin(), m_clients.end(), [&sockClient](const auto &client) { return client.Id == sockClient; });
				if (it == m_clients.end())
					m_clients.push_back(clientData);

				// Accepted someone
				if (m_onClientAccepted != nullptr)
					m_onClientAccepted(clientData.Id, clientData.Address, clientData.Port);
			}
		}
	}

	void Server::TryReceive()
	{
		LOG("Server::TryReceive()");
		int res;
		NetErrCode err;

		std::unique_lock<std::recursive_mutex> lock(m_clientsLock);

		if (m_clients.size() == 0)
		{
			// No one to receive from
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
			FD_SET(client.Id, &fds);

		// Check sockets
		res = select(0, &fds, nullptr, nullptr, &timeout);

		if (res == 0)
		{
			// No one is ready to receive
			return;
		}
		else if (res == SOCKET_ERROR)
		{
			echo("ERROR: Unknown error occurred while selecting sockets.");
			return;
		}

		// Check who is ready to receive
		auto it = m_clients.begin();
		while (it != m_clients.end())
		{
			if (!FD_ISSET(it->Id, &fds))
			{
				++it;
				continue;
			}

			int bytesReceived = recv(it->Id, m_receiveBuffer.data(), Net::GetBufferSize(), 0);
			if (bytesReceived > 0)
			{
				if (m_onReceivedFromClient != nullptr)
					m_onReceivedFromClient(it->Id, m_receiveBuffer.data(), bytesReceived);

				++it;
				continue;
			}

			if (bytesReceived == SOCKET_ERROR)
			{
				// Connection error or forced hard-close. Disconnect
				echo("ERROR: Error receiving data. Force disconnect (client id: ", it->Id, ").");
			}
			else if (bytesReceived == 0)
			{
				// Connection has been gracefully closed. Not an error, just disconnect
			}

			err = DisconnectClient_internal(it->Id);
			if (err != neterr_noErr)
				echo("ERROR: Can't disconnect client: ", it->Id, ".");

			it = m_clients.erase(it);
		}
	}


	NetErrCode Server::Cleanup()
	{
		LOG("Server::Cleanup()");
		NetErrCode err;

		{
			std::unique_lock<std::recursive_mutex> lock(m_clientsLock);

			for (auto &client : m_clients)
			{
				err = Net::CloseSocket(client.Id);
				if (err != neterr_noErr)
				{
					// No return - continue to dispose all clients
					echo("ERROR: Can't close socket (client id: ", client.Id, ").");
				}
			}

			std::vector<ClientData>().swap(m_clients);
		}

		std::vector<char>().swap(m_receiveBuffer);

		return neterr_noErr;
	}

} // ns NetLib
