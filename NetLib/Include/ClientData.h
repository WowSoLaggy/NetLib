#pragma once

#ifndef NETLIB_CLIENTDATA_H
#define NETLIB_CLIENTDATA_H


// Struct that represents client. It contains it's unique Id, address and port
struct ClientData
{
	ClientData()
	{
		Sock = 0;
		Address = "";
		Port = 0;
		Closed = false;
	}
	ClientData(SOCKET pSock, std::string pAddress, int pPort)
	{
		Sock = pSock;
		Address = pAddress;
		Port = pPort;
		Closed = false;
	}

	SOCKET Sock;			// Client's socket (and Id at the same time)
	std::string Address;	// Client's address
	int Port;				// Client's port
	bool Closed;			// Reserved flag used to determine whether the connection with this client should be closed
};


#endif // NETLIB_CLIENTDATA_H
