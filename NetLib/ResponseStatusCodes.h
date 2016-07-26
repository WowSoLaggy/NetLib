#pragma once

#ifndef NETLIB_RESPONSESTATUSCODES_H
#define NETLIB_RESPONSESTATUSCODES_H


#include <string>
#include <map>


namespace NetLib
{

	enum ResponseStatusCode : int
	{
		respcode_unknown = 0,

		respcode_continue,			// 100

		respcode_ok,				// 200

		respcode_moveperm,			// 301
		respcode_redirect,			// 302
		respcode_notmodified,		// 304

		respcode_badrequest,		// 400
		respcode_authrequired,		// 401
		respcode_forbidden,			// 403
		respcode_notfound,			// 404
		respcode_methodnotallowed,	// 405
		respcode_requesttimeout,	// 408
		respcode_uritoolarge,		// 414

		respcode_internalerror,		// 500
		respcode_methodnotimpl,		// 501
		respcode_badgateway,		// 502
		respcode_servunavailable,	// 503
		respcode_gatewaytimeout,	// 504

		respcode_end
	};

	static std::map<ResponseStatusCode, std::tuple<int, std::string>> g_responseStatusMap = {
		{ respcode_unknown, { 0, "" } },

		{ respcode_continue, { 100, "Continue" } },

		{ respcode_ok, { 200, "OK" } },

		{ respcode_moveperm, { 300, "Move Permanently" } },
		{ respcode_redirect, { 302, "Move Temporarily" } },
		{ respcode_notmodified, { 304, "Not Modified" } },

		{ respcode_badrequest, { 400, "Bad Request" } },
		{ respcode_authrequired, { 401, "Authentication Required" } },
		{ respcode_forbidden, { 403, "Forbidden" } },
		{ respcode_notfound, { 404, "Not Found" } },
		{ respcode_methodnotallowed, { 405, "Method Not Allowed" } },
		{ respcode_requesttimeout, { 408, "Request Timeout" } },
		{ respcode_uritoolarge, { 414, "Request URI too Large" } },

		{ respcode_internalerror, { 500, "Internal Server Error" } },
		{ respcode_methodnotimpl, { 501, "Method Not Implemented" } },
		{ respcode_badgateway, { 502, "Bad Gateway" } },
		{ respcode_servunavailable, { 503, "Service Unavailable" } },
		{ respcode_gatewaytimeout, { 504, "Gateway Timeout" } },

		{ respcode_end, { 0, "" } },
	};

} // ns NetLib


#endif // NETLIB_RESPONSESTATUSCODES_H
