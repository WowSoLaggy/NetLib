#pragma once

#ifndef SERVER_H
#define SERVER_H


#include "NetErrCodes.h"


namespace NetLib
{

	class Server
	{
	public:

		NetErrCode Start();
	};

} // NetLib


#endif // SERVER_H
