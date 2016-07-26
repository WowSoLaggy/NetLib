#pragma once

#ifndef NETLIB_REQUESTMETHODS_H
#define NETLIB_REQUESTMETHODS_H


#include <string>
#include <map>


namespace NetLib
{

	enum RequestMethod : int
	{
		req_unknown = 0,

		req_get,
		req_head,
		req_post,
		req_put,
		req_delete,
		req_trace,
		req_options,
		req_connect,

		req_end
	};

	static std::map<RequestMethod, std::string> g_requestMethodsMap = {
		{ req_unknown, "" },

		{ req_get, "GET" },
		{ req_head, "HEAD" },
		{ req_post, "POST" },
		{ req_put, "PUT" },
		{ req_delete, "DELETE" },
		{ req_trace, "TRACE" },
		{ req_options, "OPTIONS" },
		{ req_connect, "CONNECT" },

		{ req_end, "" },
	};

} // ns NetLib


#endif // NETLIB_REQUESTMETHODS_H
