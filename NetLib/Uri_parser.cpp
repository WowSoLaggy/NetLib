#include "Uri.h"

#include "Utils.h"


namespace NetLib
{

	void Uri::ParseFromString(const std::string &pUriString)
	{
		m_protocolStr = "";
		m_credentialsStr = "";
		m_host = "";
		m_portStr = "";
		m_path = "";
		m_parameters = "";
		m_fragment = "";

		int pos = 0;
		int curPos = 0;
		std::string str;

		// Protocol

		curPos = pUriString.find("://", pos);
		if (curPos != std::string::npos)
		{
			m_protocolStr = pUriString.substr(pos, curPos - pos);
			curPos = pos = curPos + 3;
		}
		CopyProtocolFromStr();

		// Look for user:pass@host:port/

		curPos = pUriString.find_first_of("/", pos);
		if (curPos != std::string::npos)
		{
			str = pUriString.substr(pos, curPos - pos);
			pos = curPos;
		}
		else
			str = pUriString.substr(pos);

		// Credentials

		std::vector<std::string> tokens = SplitString(str, '@');
		if (tokens.size() == 2)
		{
			m_credentialsStr = tokens[0];
			str = tokens[1];
		}
		CopyCredentialsFromStr();

		// Port & host

		tokens = SplitString(str, ':');
		if (tokens.size() > 0)
			m_host = tokens[0];
		if (tokens.size() > 1)
			m_portStr = tokens[1];
		CopyPortFromStr();

		if (pUriString[pos] != '/')
		{
			// No '/' after host:port
			return;
		}

		str = pUriString.substr(pos);
		tokens = SplitString(str, '?');
		str = tokens[0];
		if (tokens.size() > 1)
			m_parameters = tokens[1];

		tokens = SplitString(str, '#');
		m_path = tokens[0];
		if (tokens.size() > 1)
			m_fragment = tokens[1];
	}

} // NetLib
