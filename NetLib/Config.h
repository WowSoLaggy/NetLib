#pragma once

#ifndef NETLIB_CONFIG_H
#define NETLIB_CONFIG_H


#include <vector>
#include <string>
#include <map>

#include "NetErrCodes.h"
#include "HttpVersions.h"


namespace NetLib
{
	using namespace NetErrCodes;


	class Config
	{
	public:

		// Global vars

		static const std::string GetConfigFileName() { return "../Config/Config.txt"; }
		static const HttpVersion GetServerHttpVersion() { return httpver_1_1; }
		static const std::vector<HttpVersion> GetSupportedHttpVersions() { return { httpver_1_0, httpver_1_1 }; }
		static const std::string GetHttpNewLine() { return "\r\n"; }

	public:

		static NetErrCode ReadFromFile(const std::string &pFileName);


		static int GetServerPort() { return m_serverPort; }
		static bool GetKeepAliveSupport() { return m_keepAliveSupport; }
		static int GetKeepAliveTimeout() { return m_keepAliveTimeout; }
		static int GetKeepAliveMaxRequests() { return m_keepAliveMaxRequests; }
		static int GetMaxConnections() { return m_maxConnections; }

		static bool GetLogOnAccept() { return m_logOnAccept; }

		static bool GetAllowFileHandle() { return m_allowFileHandle; }
		static const std::string & GetRootFolder() { return m_rootFolder; }
		static const std::string & GetDefaultPage400() { return m_defaultPage400; }
		static const std::string & GetDefaultPage404() { return m_defaultPage404; }
		static const std::string & GetDefaultPage405() { return m_defaultPage405; }
		static const std::string & GetDefaultPage414() { return m_defaultPage414; }
		static const std::string & GetDefaultPage501() { return m_defaultPage501; }

		static const std::vector<std::string> & GetAllowedRequestMethods() { return m_allowedRequestMethods; }
		static int GetRequestUriMaxLength() { return m_requestUriMaxLength; }

		static bool GetAppendDateTimeStamp() { return m_appendDateTimeStamp; }
		static bool GetAppendServerName() { return m_appendServerName; }
		static const std::string & GetServerName() { return m_serverName; }

	private:

		static int m_serverPort;
		static bool m_keepAliveSupport;
		static int m_keepAliveTimeout;
		static int m_keepAliveMaxRequests;
		static int m_maxConnections;

		static bool m_logOnAccept;

		static bool m_allowFileHandle;
		static std::string m_rootFolder;
		static std::string m_defaultPage400;
		static std::string m_defaultPage404;
		static std::string m_defaultPage405;
		static std::string m_defaultPage414;
		static std::string m_defaultPage501;

		static std::vector<std::string> m_allowedRequestMethods;
		static int m_requestUriMaxLength;

		static bool m_appendDateTimeStamp;
		static bool m_appendServerName;
		static std::string m_serverName;

	};

} // ns NetLib


#endif // NETLIB_CONFIG_H
