#pragma once

#ifndef NETLIB_USER_H
#define NETLIB_USER_H


#include <string>
#include <vector>


struct User
{
	User(const std::string &pName, const std::string &pPassword, const std::vector<std::string> &pRealms)
		: Name(pName), Password(pPassword), Realms(pRealms)
	{ }

	std::string Name;
	std::string Password;
	std::vector<std::string> Realms;

};


#endif // NETLIB_USER_H
