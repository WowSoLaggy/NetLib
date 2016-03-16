#pragma once

#ifndef NETERRCODES_H
#define NETERRCODES_H


namespace NetLib
{

	enum NetErrCode : int
	{
		neterr_noErr = 0,
		neterr_noSocketLibrary,
		neterr_cantCreateSocket,
		neterr_cantSetSocketOptions,
		neterr_cantBindSocket,
		neterr_cantStartListen,
		neterr_cantCreateThread,

		neterr_end
	};

} // NetLib


#endif // NETERRCODES_H
