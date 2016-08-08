#pragma once

#ifndef NETLIB_AUTHTYPES_H
#define NETLIB_AUTHTYPES_H


#include <string>
#include <map>


namespace NetLib
{

	// Enumeration of known Authorization types
	enum AuthType : int
	{
		auth_unknown = 0,

		auth_none,
		auth_basic,
		auth_digest,

		auth_end
	};

	// Map that matches the Authorization type and it's string declaration
	static std::map<AuthType, std::string> g_authTypesMap = {
		{ auth_unknown, "" },

		{ auth_none, "none" },
		{ auth_basic, "basic" },
		{ auth_digest, "digest" },

		{ auth_end, "" },
	};

} // ns NetLib


#endif // NETLIB_AUTHTYPES_H
