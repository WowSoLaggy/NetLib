#pragma once

#ifndef NETLIB_ACCESSTYPES_H
#define NETLIB_ACCESSTYPES_H


#include <map>
#include <string>


namespace NetLib
{

	// Enumeration of known access types
	enum AccessType : int
	{
		access_unknown = 0,

		access_read,
		access_write,

		access_end
	};

	// Map that matches the Authorization type and it's string declaration
	static std::map<AccessType, std::string> g_accessTypesMap = {
		{ access_unknown, "" },

		{ access_read, "read" },
		{ access_write, "write" },

		{ access_end, "" },
	};

} // ns NetLib


#endif // NETLIB_ACCESSTYPES_H
