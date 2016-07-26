#pragma once

#ifndef NETLIB_HTTPVERSIONS_H
#define NETLIB_HTTPVERSIONS_H


#include <string>
#include <map>


namespace NetLib
{

	enum HttpVersion : int
	{
		httpver_unknown = 0,

		httpver_1_0,
		httpver_1_1,

		httpver_end
	};

	static std::map<HttpVersion, std::string> g_httpVersionsMap = {
		{ httpver_unknown, "" },

		{ httpver_1_0, "HTTP/1.0" },
		{ httpver_1_1, "HTTP/1.1" },

		{ httpver_end, "" },
	};

} // ns NetLib


#endif // NETLIB_HTTPVERSIONS_H
