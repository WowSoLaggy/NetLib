#pragma once

#ifndef NETLIB_AUTH_H
#define NETLIB_AUTH_H


#include "NetErrCodes.h"
#include "AuthTypes.h"


namespace NetLib
{
	using namespace NetErrCodes;


	// Class used to authorize requests with an access file.
	// Supports different types of Http authorization (none, basic, digest)
	class Auth
	{
	public:

		// Returns current authorization type
		AuthType GetAuthType() const { return m_authType; }


		// Updates all authorization info from the config files
		NetErrCode UpdatePrivileges();

		bool Authorize();

	private:

		// Returns current authorization type
		AuthType m_authType;

	};

} // ns NetLib


#endif // NETLIB_AUTH_H
