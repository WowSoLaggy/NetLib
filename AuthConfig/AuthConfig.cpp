#include "stdafx.h"
#include "AuthConfig.h"

#include "Utils.h"


int AuthConfig::Run()
{
	ShowMainMenu();

	while (true)
	{
		std::cout << "> ";

		char key = (char)_getch();
		std::cout << key << std::endl;
		if (key == '7')
			break;

		else if (key == '1')
			ListRealms();
		else if (key == '2')
			AddRealm();
		else if (key == '3')
			RemoveRealm();
		else if (key == '4')
			ListUsers();
		else if (key == '5')
			AddUser();
		else if (key == '6')
			RemoveUser();
		else if (key == '?')
			ShowMainMenu();
		else
			UnrecognizedCmd();
	}

	return 0;
}


void AuthConfig::ShowMainMenu()
{
	std::cout << "Welcome to NetLib AuthConfig." << std::endl << std::endl;
	std::cout << "Main menu:" << std::endl << std::endl;

	std::cout << "   1 - List realms" << std::endl;
	std::cout << "   2 - Add realm" << std::endl;
	std::cout << "   3 - Remove realm" << std::endl << std::endl;

	std::cout << "   4 - List users" << std::endl;
	std::cout << "   5 - Add user" << std::endl;
	std::cout << "   6 - Remove user" << std::endl << std::endl;

	std::cout << "   7 - Exit" << std::endl;
	std::cout << "   ? - This menu" << std::endl << std::endl;
}


void AuthConfig::ReadFromFile()
{
	m_realms.clear();
	m_users.clear();

	m_maxUserNameLength = 5;
	m_maxRealmName = 5;

	if (!NetLib::CheckFileExists(c_configFileName))
	{
		GenerateDefaultFile();
		return;
	}

	int mode = 0; // 1 - reading realms, 2 - reading users

	std::fstream f(c_configFileName, std::ios::in);
	std::string line;
	std::vector<std::string> tokens;
	while (std::getline(f, line))
	{
		line = NetLib::TrimString(line);
		if (line[0] == ';')
			continue;

		if (line.compare(c_realmsOpenStr) == 0)
		{
			mode = 1;
			continue;
		}
		else if (line.compare(c_realmsCloseStr) == 0)
		{
			mode = 0;
			continue;
		}
		else if (line.compare(c_usersOpenStr) == 0)
		{
			mode = 2;
			continue;
		}
		else if (line.compare(c_usersCloseStr) == 0)
		{
			mode = 0;
			continue;
		}

		if (mode == 1)
		{
			m_realms.emplace_back(line);
			if ((int)line.size() > m_maxRealmName)
				m_maxRealmName = (int)line.size();
		}
		else if (mode == 2)
		{
			tokens = NetLib::SplitString(line, ':');
			if (tokens.size() != 3)
				continue;

			m_users.push_back({ tokens[0], tokens[1], tokens[2] });
			if ((int)tokens[0].size() > m_maxUserNameLength)
				m_maxUserNameLength = (int)tokens[0].size();
		}
	}
}

void AuthConfig::WriteToFile()
{
	GenerateDefaultFile();

	std::fstream f(c_configFileName, std::ios::app | std::ios::out);

	f << std::endl <<
		"; -=-=-=-=-=-=-=-=-=-=-=-=-" << std::endl <<
		"; REALMS" << std::endl <<
		"; -=-=-=-=-=-=-=-=-=-=-=-=-" << std::endl << std::endl << std::endl;

	f << c_realmsOpenStr << std::endl;
	for (auto & realm : m_realms)
		f << "   " << realm << std::endl;
	f << c_realmsCloseStr << std::endl << std::endl;

	f << std::endl <<
		"; -=-=-=-=-=-=-=-=-=-=-=-=-" << std::endl <<
		"; USERS" << std::endl <<
		"; -=-=-=-=-=-=-=-=-=-=-=-=-" << std::endl << std::endl << std::endl;

	f << c_usersOpenStr << std::endl;
	for (auto & user : m_users)
		f << "   " << user.Name << ":" << user.Realm << ":" << user.Password << std::endl;
	f << c_usersCloseStr << std::endl;
}

void AuthConfig::GenerateDefaultFile()
{
	std::fstream f(c_configFileName, std::ios::trunc | std::ios::out);
	f <<
		"; This file is auto generated." << std::endl <<
		"; Do not modify it if you are not competent." << std::endl <<
		";" << std::endl <<
		"; NetLib HttpServer authorization file." << std::endl <<
		"; This file contains authorization rules for server resources." << std::endl <<
		"; Lines starting with ';' are comments and are ignored." << std::endl <<
		";" << std::endl << std::endl;
}


void AuthConfig::UnrecognizedCmd()
{
	std::cout << "Unrecognized command." << std::endl;
	ShowMainMenu();
}


void AuthConfig::ListRealms()
{
	ReadFromFile();

	if (m_realms.size() == 0)
	{
		std::cout << "No realms to show." << std::endl << std::endl;
		return;
	}

	std::cout << std::endl << "Available realms:" << std::endl;
	for (int i = 0; i < (int)m_realms.size(); ++i)
		std::cout << "   " << (i + 1) << ". " << m_realms[i] << std::endl;

	std::cout << std::endl << "Total " << m_realms.size() << " realms." << std::endl << std::endl;
}

void AuthConfig::AddRealm()
{
	ReadFromFile();

	std::string realmName;
	std::cout << "Valid symbols in the realm name: [A-Z], [a-z], [0-9], '-', '_'." << std::endl;
	std::cout << "Enter new realm name:" << std::endl << "> ";
	std::getline(std::cin, realmName);

	if ((realmName.size() == 0) || (realmName.size() > 16))
	{
		std::cout << "ERROR: Invalid realm name length (1..16 symbols)." << std::endl << std::endl;
		return;
	}

	if (!std::regex_match(realmName, std::regex("[A-Za-z0-9-_]+")))
	{
		std::cout << "ERROR: Invalid symbols in the realm name." << std::endl << std::endl;
		return;
	}

	m_realms.emplace_back(realmName);
	WriteToFile();
	std::cout << std::endl << "Realm \"" << realmName << "\" successfully added." << std::endl << std::endl;
}

void AuthConfig::RemoveRealm()
{
	ReadFromFile();

	if (m_realms.size() == 0)
	{
		std::cout << "No realms to remove." << std::endl << std::endl;
		return;
	}

	std::cout << std::endl << "Available realms:" << std::endl;
	for (int i = 0; i < (int)m_realms.size(); ++i)
		std::cout << "   " << (i + 1) << ". " << m_realms[i] << std::endl;

	std::string realmName;
	std::cout << "Enter realm name to delete:" << std::endl << "> ";
	std::getline(std::cin, realmName);
	auto it = std::find(m_realms.begin(), m_realms.end(), realmName);
	if (it == m_realms.end())
	{
		std::cout << "ERROR: Can't find the realm with the entered name." << std::endl << std::endl;
		return;
	}

	std::cout << "All users privileged to this realm can lose access to it's resources." << std::endl;
	std::cout << "Are you sure you want to remove \"" << (*it) << "\" realm? [y/n]" << std::endl << "> ";
	char key = (char)_getch();
	if (key == 'y')
	{
		m_realms.erase(it);
		WriteToFile();
		std::cout << "Realm successfully removed." << std::endl << std::endl;
	}
	else if (key == 'n')
		std::cout << "Realm removal CANCELED." << std::endl << std::endl;
	else
		std::cout << "Token not recognized. Realm removal CANCELED." << std::endl << std::endl;
}


void AuthConfig::ListUsers()
{
	ReadFromFile();

	if (m_users.size() == 0)
	{
		std::cout << "No users to show." << std::endl << std::endl;
		return;
	}

	std::cout << std::endl << "Available users:" << std::endl << std::endl;
	std::cout << std::setw(m_maxUserNameLength + 7) << "Name" << " | " << std::setw(m_maxRealmName) << "Realm" << " | Password secured" << std::endl;
	std::cout << std::setfill('-') << std::setw(80) << " " << std::setfill(' ') << std::endl;
	for (int i = 0; i < (int)m_users.size(); ++i)
		std::cout << "   " << std::setw(2) << (i + 1) << ". " << std::setw(m_maxUserNameLength) << m_users[i].Name << " | " <<
		std::setw(m_maxRealmName) << m_users[i].Realm << " | " <<
		(m_users[i].Password.empty() ? "No" : "Yes") << std::endl;

	std::cout << std::endl << "Total " << m_users.size() << " users." << std::endl << std::endl;
}

void AuthConfig::AddUser()
{
	ReadFromFile();

	// User name

	std::string userName;
	std::cout << "Valid symbols in the user name: [A-Z], [a-z], [0-9], '-', '_'." << std::endl;
	std::cout << "Enter new user name:" << std::endl << "> ";
	std::getline(std::cin, userName);

	if ((userName.size() == 0) || (userName.size() > 16))
	{
		std::cout << "ERROR: Invalid user name length (1..16 symbols)." << std::endl << std::endl;
		return;
	}

	if (!std::regex_match(userName, std::regex("[A-Za-z0-9-_]+")))
	{
		std::cout << "ERROR: Invalid symbols in the user name." << std::endl << std::endl;
		return;
	}

	// Realm

	std::string realmName = "";

	if (m_realms.size() != 0)
	{
		std::cout << "Enter the name of the existing realm to add this user to." << std::endl;
		std::cout << "Leave blank to not to specify realm (can be changed later).";
		std::cout << std::endl << "Available realms:" << std::endl;
		for (int i = 0; i < (int)m_realms.size(); ++i)
			std::cout << "   " << (i + 1) << ". " << m_realms[i] << std::endl;

		std::cout << "Enter realm name:" << std::endl << "> ";
		std::getline(std::cin, realmName);
		if (realmName.size() > 0)
		{
			auto it = std::find(m_realms.begin(), m_realms.end(), realmName);
			if (it == m_realms.end())
			{
				std::cout << "ERROR: Can't find the realm with the entered name." << std::endl << std::endl;
				return;
			}
		}
	}
	else
		std::cout << "No realms found to add user to." << std::endl << std::endl;

	// Password

	std::string password1;
	std::string password2;
	std::cout << "Valid password symbols: [A-Z], [a-z], [0-9], '-', '_', '!', '@', '#'." << std::endl;
	std::cout << "Enter password: ";
	std::getline(std::cin, password1);

	if ((password1.size() == 0) || (password1.size() > 16))
	{
		std::cout << "ERROR: Invalid password length (1..16 symbols)." << std::endl << std::endl;
		return;
	}

	if (!std::regex_match(password1, std::regex("[A-Za-z0-9-_!@#]+")))
	{
		std::cout << "ERROR: Invalid symbols in the password." << std::endl << std::endl;
		return;
	}

	std::cout << "Repeat password: ";
	std::getline(std::cin, password2);
	if (password1.compare(password2) != 0)
	{
		std::cout << "ERROR: Passwords are not the same." << std::endl << std::endl;
		return;
	}

	// Finalize

	m_users.push_back({ userName, realmName, password1 });
	WriteToFile();
	std::cout << std::endl << "User \"" << userName << "\" successfully added." << std::endl << std::endl;
}

void AuthConfig::RemoveUser()
{
	ReadFromFile();

	if (m_users.size() == 0)
	{
		std::cout << "No users to remove." << std::endl << std::endl;
		return;
	}

	std::cout << std::endl << "Available users:" << std::endl;
	for (int i = 0; i < (int)m_users.size(); ++i)
		std::cout << "   " << (i + 1) << ". " << m_users[i].Name << std::endl;

	std::string userName;
	std::cout << "Enter user name to delete:" << std::endl << "> ";
	std::getline(std::cin, userName);
	auto it = std::find_if(m_users.begin(), m_users.end(), [&](auto user) { return user.Name.compare(userName) == 0; } );
	if (it == m_users.end())
	{
		std::cout << "ERROR: Can't find the user with the entered name." << std::endl << std::endl;
		return;
	}

	std::cout << "Are you sure you want to remove \"" << it->Name << "\" user? [y/n]" << std::endl << "> ";
	char key = (char)_getch();
	if (key == 'y')
	{
		m_users.erase(it);
		WriteToFile();
		std::cout << "User successfully removed." << std::endl << std::endl;
	}
	else if (key == 'n')
		std::cout << "User removal CANCELED." << std::endl << std::endl;
	else
		std::cout << "Token not recognized. User removal CANCELED." << std::endl << std::endl;
}
