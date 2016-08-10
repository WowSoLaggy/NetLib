#pragma once

#ifndef AUTHCONFIG_AUTHCONFIG_H
#define AUTHCONFIG_AUTHCONFIG_H


#include <string>
#include <vector>

#include "User.h"


class AuthConfig
{
public:

	int Run();

private:

	const std::string c_realmsOpenStr = "<Realms>";
	const std::string c_realmsCloseStr = "</Realms>";
	const std::string c_usersOpenStr = "<Users>";
	const std::string c_usersCloseStr = "</Users>";

	const std::string c_configFileName = "Auth.conf";

	int m_maxUserNameLength = 16;
	int m_maxRealmName = 16;

	std::vector<std::string> m_realms;
	std::vector<User> m_users;

	void ShowMainMenu();

	void ReadFromFile();
	void WriteToFile();
	void GenerateDefaultFile();

	void UnrecognizedCmd();

	void ListRealms();
	void AddRealm();
	void RemoveRealm();

	void ListUsers();
	void AddUser();
	void RemoveUser();
	void ChangeUserRealm();
	void ChangeUserPassword();

	void PrintUsersList();

};


#endif // AUTHCONFIG_AUTHCONFIG_H
