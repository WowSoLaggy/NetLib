#pragma once

#ifndef NETERRCODES_H
#define NETERRCODES_H


namespace NetLib
{

	enum NetErrCode : int
	{
		neterr_noErr = 0,
		neterr_noWsa,
		neterr_cantCleanupWsa,
		neterr_cantCreateSocket,
		neterr_cantSetSocketOptions,
		neterr_cantBindSocket,
		neterr_cantStartListen,
		neterr_cantCreateThread,
		neterr_cantConnect,
		neterr_connectionRefused,
		neterr_connectionTimedOut,
		neterr_cantSend,
		neterr_connectionClosed,
		neterr_noSuchClient,
		neterr_errorDisconnecting,

		neterr_end
	};

} // NetLib


#endif // NETERRCODES_H
