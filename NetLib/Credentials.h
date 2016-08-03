#pragma once

#ifndef NETLIB_CREDENTIALS_H
#define NETLIB_CREDENTIALS_H


#include <string>


namespace NetLib
{

	// Struct that stores credentials
	struct Credentials
	{
		Credentials()
			: UserName(""), Password("")
		{
		}

		Credentials(const std::string &pUserName, const std::string &pPassword)
			: UserName(pUserName), Password(pPassword)
		{
		}

		std::string UserName;	// User name
		std::string Password;	// Password
	};

} // ns NetLib


#endif // NETLIB_CREDENTIALS_H
