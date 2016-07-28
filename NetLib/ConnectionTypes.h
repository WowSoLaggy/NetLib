#pragma once

#ifndef NETLIB_CONNECTIONTYPES_H
#define NETLIB_CONNECTIONTYPES_H


namespace NetLib
{

	enum ConnectionType
	{
		con_unknown = 0,

		con_close,
		con_keepalive,

		con_end
	};

} // ns netLib


#endif // NETLIB_CONNECTIONTYPES_H
