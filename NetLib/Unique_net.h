#pragma once

#ifndef NETLIB_UNIQUE_NET_H
#define NETLIB_UNIQUE_NET_H


#include "Net.h"


// Creates NetLib RAII object. All NetLib classes and functions should be called only while the lifetime of this container
#define NETINIT ::NetLib::unique_net unique_net


namespace NetLib
{

	// RAII container for NetLib. Initializes static members in the ctor and releases in the dtor.
	// The NetLib class should be called only while the lifetime of this container
	class unique_net final
	{
	public:

		// Ctor for common network initialization
		unique_net()
		{
			Net::Init();
		}

		// Dtor for common network disposion
		~unique_net()
		{
			Net::Dispose();
		}
	};

} // ns NetLib


#endif // NETLIB_UNIQUE_NET_H
