#pragma once

#ifndef NETLIB_AUTH_H
#define NETLIB_AUTH_H


#include <vector>
#include <string>
#include <filesystem>
#include <map>

#include "NetErrCodes.h"
#include "AuthTypes.h"
#include "AccessTypes.h"
#include "HttpConnectionInfo.h"
#include "User.h"


namespace NetLib
{
	using namespace NetErrCodes;
	using std::tr2::sys::path;


	// Class used to authorize requests with an access file.
	// Supports different types of Http authorization (none, basic, digest)
	class Auth
	{
	public:

		// Returns current authorization type
		AuthType GetAuthType() const { return m_authType; }


		// Updates all authorization info from the config files
		NetErrCode UpdatePrivileges();

		// Returns whether the given client is authorized to access the given resource
		// Params:
		// [in]  const HttpConnectionInfo & pHttpConnectionInfo	- information about the connection to grant access to
		// [in]  const path & pResourceUri						- path to the required resource
		// [in]  AccessType pAccessType							- type of the required access
		// [out] std::string & pAuthorizationHeader				- authorization header that is returned if the authorization is required
		NetErrCode CanAccess(const HttpConnectionInfo &pHttpConnectionInfo, const path &pResourceUri, AccessType pAccessType,
			std::string &pAuthorizationHeader);

	private:

		// Literal consts for the file parsing
		const std::string c_realmsOpenStr = "<Realms>";
		const std::string c_realmsCloseStr = "</Realms>";
		const std::string c_usersOpenStr = "<Users>";
		const std::string c_usersCloseStr = "</Users>";

		// Current authorization type
		AuthType m_authType;

		// List of realms loaded from the auth file
		std::vector<std::string> m_realms;

		// List of users loaded from the auth file
		std::vector<User> m_users;

		// Reads realms and users from the auth file
		NetErrCode ReadPrivilegesFromFile();

		// Returns the map of access types and corresponding realms from the given .htaccess file
		std::map<AccessType, std::string> ReadHtAccessFile(const std::string &pHtAccessFilePath);

		// Returns the path to the corresponding .htaccess file for the given directory.
		// This file can be in the parent dir or taken by default
		std::string FindHtAccessFile(const path &pDir);

		// Returns authorization string that is written as header if the authorization is required
		std::string GetAuthorizationString(const std::string &pRealm);

		// Checks authentication info, return false if credentials are invalid
		bool Authorize(const std::string &pAuthString, const std::string &pRequiredRealm);

		// Checks basic authentication info, return false if credentials are invalid
		bool AuthorizeBasic(const std::string &pAuthString, const std::string &pRequiredRealm);

		// Checks digest authentication info, return false if credentials are invalid
		bool AuthorizeDigest(const std::string &pAuthString, const std::string &pRequiredRealm);

	};

} // ns NetLib


#endif // NETLIB_AUTH_H
