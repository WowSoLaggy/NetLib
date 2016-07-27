#include "Config.h"

#include <fstream>
#include <sstream>

#include "Utils.h"


namespace NetLib
{

	int Config::m_serverPort;

	std::string Config::m_rootFolder;

	std::vector<std::string> Config::m_allowedRequestMethods;
	int Config::m_requestUriMaxLength;

	bool Config::m_appendDateTimeStamp;
	bool Config::m_appendServerName;
	std::string Config::m_serverName;


	NetErrCode Config::ReadFromFile(const std::string &pFileName)
	{
		std::fstream f(pFileName, std::ios::in);
		std::string line;
		std::vector<std::string> tokens;

		std::string header;
		std::string value;

		while (std::getline(f, line))
		{
			TrimString(line);
			if (line.empty())
				continue;
			if (line[0] == ';')
				continue;

			int pos = line.find_first_of('=', 0);
			if ((pos == std::string::npos) || (pos == 0))
				continue;

			header = TrimString(line.substr(0, pos));
			value = TrimString(line.substr(pos + 1));

			if (header.compare("ServerPort") == 0)
				m_serverPort = std::stoi(value);
			else if (header.compare("RootFolder") == 0)
				m_rootFolder = value;
			else if (header.compare("AppendDateTime") == 0)
				m_appendDateTimeStamp = StringToBool(value);
			else if (header.compare("AppendServerName") == 0)
				m_appendServerName = StringToBool(value);
			else if (header.compare("ServerName") == 0)
				m_serverName = value;
			else if (header.compare("AllowedRequestMethods") == 0)
			{
				tokens = SplitString(value, ',');
				for (auto & name : tokens)
					m_allowedRequestMethods.push_back(TrimString(name));
			}
			else if (header.compare("RequestUriMaxLength") == 0)
				m_requestUriMaxLength = std::stoi(value);
			else
			{
				// Log unrecognized token
			}
		}

		return neterr_noErr;
	}

} // ns NetLib
