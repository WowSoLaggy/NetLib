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

	struct HttpConnectionInfo
	{
		HttpConnectionInfo()
			: Id(0), ConnectType(con_unknown), RequestsCount(0)
		{
			ConnectTimer.Start();
		}

		HttpConnectionInfo(CLIENTID pId)
			: Id(pId), ConnectType(con_unknown), RequestsCount(0)
		{
			ConnectTimer.Start();
		}

		CLIENTID Id;

		ConnectionType ConnectType;
		Timer ConnectTimer;
		int RequestsCount;

	};

} // ns Netlib


#endif // NETLIB_HTTPCONNECTIONINFO_H
