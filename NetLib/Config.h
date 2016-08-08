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


	// Class that provides an access to the HttpServer's configuration
	class Config
	{
	public:


		// Global vars


		// Returns the Http version that is supported by the HttpServer
		static const HttpVersion GetServerHttpVersion() { return httpver_1_1; }

		// Returns the supported Http versions
		static const std::vector<HttpVersion> GetSupportedHttpVersions() { return { httpver_1_0, httpver_1_1 }; }

		// Returns symbols of a new line
		static const std::string GetHttpNewLine() { return "\r\n"; }

	public:


		// Reads configuration from the given file
		// Params:
		// [in] const std::string & pFileName	- name of the file to read configuration from
		static NetErrCode ReadFromFile(const std::string &pFileName);


		// Port to start the HttpServer on
		static int GetServerPort() { return m_serverPort; }

		// Keep-Alive support (HTTP/1.1 should support Keep-Alive by default)
		static bool GetKeepAliveSupport() { return m_keepAliveSupport; }

		// Keep - Alive timeout in seconds
		static int GetKeepAliveTimeout() { return m_keepAliveTimeout; }

		// Keep-Alive max requests count
		static int GetKeepAliveMaxRequests() { return m_keepAliveMaxRequests; }

		// Max number of connections(connected clients)
		static int GetMaxConnections() { return m_maxConnections; }


		// Authorization type
		static std::string GetAuthType() { return m_authType; }


		// Log accepted connections (can spam the log file)
		static bool GetLogOnAccept() { return m_logOnAccept; }


		// Allow requests to the file system.
		// If false then requests such as GET, PUT, etc will be passed as a callback to the control application
		static bool GetAllowFileHandle() { return m_allowFileHandle; }
		
		// Server root folder. If 'AllowFileHandle == false' used only to get default pages (DefaultPage404, etc...)
		static const std::string & GetRootFolder() { return m_rootFolder; }

		// Default file to get if the Uri points to the directory
		static const std::string & GetDefaultIndex() { return m_defaultIndex; }

		// Default 400 (Bad Request) page
		static const std::string & GetDefaultPage400() { return m_defaultPage400; }

		// Default 401 (Auth Required) page
		static const std::string & GetDefaultPage401() { return m_defaultPage401; }

		// Default 403 (Forbidden) page
		static const std::string & GetDefaultPage403() { return m_defaultPage403; }

		// Default 404 (Not Found) page
		static const std::string & GetDefaultPage404() { return m_defaultPage404; }

		// Default 405 (Method Not Allowed) page
		static const std::string & GetDefaultPage405() { return m_defaultPage405; }

		// Default 414 (Uri Too Large) page
		static const std::string & GetDefaultPage414() { return m_defaultPage414; }

		// Default 500 (Internal Server Error) page
		static const std::string & GetDefaultPage500() { return m_defaultPage500; }

		// Default 501 (Method Not Implemented) page
		static const std::string & GetDefaultPage501() { return m_defaultPage501; }


		// Allowed request methods (other request methods will generate MethodNotAllowed response)
		static const std::vector<std::string> & GetAllowedRequestMethods() { return m_allowedRequestMethods; }
		
		// Request URI max length
		static int GetRequestUriMaxLength() { return m_requestUriMaxLength; }

		// Request body max length
		static int GetRequestBodyMaxLength() { return m_requestBodyMaxLength; }


		// Append DateTime stamp to all outgoing responses
		static bool GetAppendDateTimeStamp() { return m_appendDateTimeStamp; }

		// Append server version to all outgoing responses
		static bool GetAppendServerName() { return m_appendServerName; }

		// Server name to append to all outgoing responses. Ignored if 'AppendServerName = false'
		static const std::string & GetServerName() { return m_serverName; }

	private:

		// Port to start the HttpServer on
		static int m_serverPort;

		// Keep-Alive support (HTTP/1.1 should support Keep-Alive by default)
		static bool m_keepAliveSupport;

		// Keep - Alive timeout in seconds
		static int m_keepAliveTimeout;

		// Keep-Alive max requests count
		static int m_keepAliveMaxRequests;

		// Max number of connections(connected clients)
		static int m_maxConnections;


		// Authorization type
		static std::string m_authType;


		// Log accepted connections (can spam the log file)
		static bool m_logOnAccept;


		// Allow requests to the file system.
		// If false then requests such as GET, PUT, etc will be passed as a callback to the control application
		static bool m_allowFileHandle;

		// Server root folder. If 'AllowFileHandle == false' used only to get default pages (DefaultPage404, etc...)
		static std::string m_rootFolder;

		// Default file to get if the Uri points to the directory
		static std::string m_defaultIndex;

		// Default 400 (Bad Request) page
		static std::string m_defaultPage400;

		// Default 401 (Auth Required) page
		static std::string m_defaultPage401;

		// Default 403 (Forbidden) page
		static std::string m_defaultPage403;

		// Default 404 (Not Found) page
		static std::string m_defaultPage404;

		// Default 405 (Method Not Allowed) page
		static std::string m_defaultPage405;

		// Default 414 (Uri Too Large) page
		static std::string m_defaultPage414;

		// Default 500 (Internal Server Error) page
		static std::string m_defaultPage500;

		// Default 501 (Method Not Implemented) page
		static std::string m_defaultPage501;


		// Allowed request methods (other request methods will generate MethodNotAllowed response)
		static std::vector<std::string> m_allowedRequestMethods;

		// Request URI max length
		static int m_requestUriMaxLength;

		// Request body max length
		static int m_requestBodyMaxLength;


		// Append DateTime stamp to all outgoing responses
		static bool m_appendDateTimeStamp;

		// Append server version to all outgoing responses
		static bool m_appendServerName;

		// Server name to append to all outgoing responses. Ignored if 'AppendServerName = false'
		static std::string m_serverName;

	};

} // ns NetLib


#endif // NETLIB_CONFIG_H
