#pragma once

#ifndef CLIENTDATA_H
#define CLIENTDATA_H


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

	SOCKET Sock;
	std::string Address;
	int Port;
	bool Closed;
};


#endif // CLIENTDATA_H
