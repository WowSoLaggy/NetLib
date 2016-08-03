#pragma once

#ifndef NETLIB_PROTOCOLS_H
#define NETLIB_PROTOCOLS_H


#include <string>
#include <map>


namespace NetLib
{

	// Enumeration of known protocols
	enum Protocol : int
	{
		prot_unknown = 0,

		prot_http,
		prot_ftp,
		prot_rtsp,

		prot_end
	};

	// Map that matches Protocol and it's string declaration
	static std::map<Protocol, std::string> g_protocolMap = {
		{ prot_unknown, "" },

		{ prot_http, "http" },
		{ prot_ftp, "ftp" },
		{ prot_rtsp, "rtsp" },

		{ prot_end, "" }
	};

} // ns NetLib


#endif // NETLIB_PROTOCOLS_H
