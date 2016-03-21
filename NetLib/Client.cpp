#include "stdafx.h"
#include "Client.h"


namespace NetLib
{

	Client::Client() { }
	Client::~Client() { }


	NetErrCode Client::Connect(std::string pServerIp, int pServerPort)
	{
		LOG("Client::Connect()");
		NetErrCode err;
		int res;

		// Create socket

		err = Net::CreateSocket(m_sockClient);
		if (err != neterr_noErr)
		{
			echo("Can't create socket to listen.");
			return err;
		}

		// Set option SO_REUSEADDR

		err = Net::SetOptReuseAddr(m_sockClient);
		if (err != neterr_noErr)
		{
			echo("Can't set socket SO_REUSEADDR option.");
			Net::CloseSocket(m_sockClient);
			return err;
		}

		// Connect to server

		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr(pServerIp.c_str());
		addr.sin_port = htons(pServerPort);
		res = connect(m_sockClient, (const sockaddr *)&addr, sizeof(sockaddr_in));
		if (res != 0)
		{
			echo("Can't connect to the server: ", pServerIp, ":", pServerPort);
			Net::CloseSocket(m_sockClient);
			return neterr_cantConnect;
		}

		return neterr_noErr;
	}

	NetErrCode Client::Disconnect()
	{
		Net::CloseSocket(m_sockClient);
		return neterr_noErr;
	}

} // NetLib
