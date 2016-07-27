#include "Config.h"

#include <fstream>
#include <sstream>

#include "Utils.h"


namespace NetLib
{

	int Config::m_serverPort;
	std::string Config::m_rootFolder;
	bool Config::m_appendDateTimeStamp;
	bool Config::m_appendServerVersion;


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
			else if (header.compare("AppendServerVersion") == 0)
				m_appendServerVersion = StringToBool(value);
			else
			{
				// Log unrecognized token
			}
		}

		return neterr_noErr;
	}

} // ns NetLib
