#pragma once

#ifndef NETLIB_HTTPCONNECTIONINFO_H
#define NETLIB_HTTPCONNECTIONINFO_H


#include <Windows.h>
#include <string>
#include "Timer.h"

#include "ClientInfo.h"
#include "ConnectionTypes.h"


namespace NetLib
{

	// Struct that contains information about the http connection
	struct HttpConnectionInfo
	{
	public:

		HttpConnectionInfo()
			: Id(0), ConnectType(con_unknown), RequestsCount(0)
		{
			m_connectTimer.Start();
		}

		HttpConnectionInfo(CLIENTID pId)
			: Id(pId), ConnectType(con_unknown), RequestsCount(0)
		{
			m_connectTimer.Start();
		}

		// Returns the time passed after the connection established
		double GetConnectionTime() { return m_connectTimer.Check(); }

		CLIENTID Id;				// Id of the client that connected via this connection
		ConnectionType ConnectType;	// Type of the connection (keep-alive, close, ...)
		int RequestsCount;			// Maximum number of requests from this connection before it will be closed

	private:

		Timer m_connectTimer;		// Timer to get the time passed after the connection established

	};

} // ns Netlib


#endif // NETLIB_HTTPCONNECTIONINFO_H
