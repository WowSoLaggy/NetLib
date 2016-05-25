#pragma once

#ifndef NETLIB_CLIENTDATA_H
#define NETLIB_CLIENTDATA_H


typedef UINT_PTR CLIENTID;


// Struct that represents client. It contains it's unique Id, address and port
struct ClientData
{
	ClientData()
	{
		Id = 0;
		Address = "";
		Port = 0;
		Closed = false;
	}
	ClientData(CLIENTID pId, std::string pAddress, int pPort)
	{
		Id = pId;
		Address = pAddress;
		Port = pPort;
		Closed = false;
	}

	CLIENTID Id;		// Client's Id
	std::string Address;	// Client's address
	int Port;				// Client's port
	bool Closed;			// Reserved flag used to determine whether the connection with this client should be closed
};


#endif // NETLIB_CLIENTDATA_H
