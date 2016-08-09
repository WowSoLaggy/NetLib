#pragma once

#ifndef AUTHCONFIG_USER_H
#define AUTHCONFIG_USER_H


#include <string>


struct User
{
	User(const std::string &pName, const std::string &pRealm, const std::string &pPassword)
		: Name(pName), Realm(pRealm), Password(pPassword)
	{ }

	std::string Name;
	std::string Realm;
	std::string Password;
};


#endif // AUTHCONFIG_USER_H
