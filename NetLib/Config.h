#pragma once

#ifndef NETLIB_CONFIG_H
#define NETLIB_CONFIG_H


#include <string>
#include <map>

#include "NetErrCodes.h"


namespace NetLib
{
	using namespace NetErrCodes;


	class Config
	{
	public:

		// Global vars

		static std::string GetConfigFileName() { return "../Config/Config.txt"; }

	public:

		static NetErrCode ReadFromFile(const std::string &pFileName);

		static int GetServerPort() { return m_serverPort; }
		static const std::string & GetRootFolder() { return m_rootFolder; }
		static bool GetAppendDateTimeStamp() { return m_appendDateTimeStamp; }
		static bool GetAppendServerVersion() { return m_appendServerVersion; }

	private:

		static int m_serverPort;
		static std::string m_rootFolder;
		static bool m_appendDateTimeStamp;
		static bool m_appendServerVersion;

	};

} // ns NetLib


#endif // NETLIB_CONFIG_H
