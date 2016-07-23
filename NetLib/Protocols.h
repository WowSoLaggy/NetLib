#pragma once

#ifndef NETLIB_PROTOCOLS_H
#define NETLIB_PROTOCOLS_H


namespace NetLib
{

	enum Protocol : int
	{
		prot_unknown = 0,

		prot_http,
		prot_ftp,
		prot_rtsp,

		prot_end
	};

	static const std::string g_ProtocolStrings[] = {
		"",
		"http",
		"ftp",
		"rtsp",

		""
	};

} // ns NetLib


#endif // NETLIB_PROTOCOLS_H
