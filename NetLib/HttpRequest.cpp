#include "HttpRequest.h"


namespace NetLib
{
	using namespace NetErrCodes;


	NetErrCode HttpRequest::Parse(const std::string &pRequestString, int &pOffset)
	{
		std::string str;
		std::vector<std::string> tokens;
		pOffset = 0;
		int res = 0;
		
		// Get request line
		res = pRequestString.find("\r\n", pOffset);
		if (res == std::string::npos)
			return neterr_parse_cantParse;
		if (res > 255) // TODO: move 255 to config
			return neterr_parse_requestLineTooLong;
		pOffset += res + 2; // skip \r\n

		// Parse request line

		str = pRequestString.substr(0, res);
		tokens = SplitString(str, ' ');
		if (tokens.size() != 3)
			return neterr_parse_cantParse;

		// Request method

		for (auto & method : g_requestMethodsMap)
		{
			if (tokens[0].compare(method.second) == 0)
			{
				m_method = method.first;
				break;
			}
			if (method.first == req_end)
				return neterr_parse_cantParse;
		}

		// Request Uri

		m_uri = Uri(tokens[1]);

		// Request Http version

		for (auto & version : g_httpVersionsMap)
		{
			if (tokens[2].compare(version.second) == 0)
			{
				m_httpVersion = version.first;
				break;
			}
			if (version.first == httpver_end)
				return neterr_parse_cantParse;
		}

		//if (m_httpVersion != httpver_1_0) // TODO: move supported http version to config
		//	return neterr_parse_cantParse;

		// Skip request headers and body

		int emptyLines = 0;
		while (emptyLines < 1)	// TODO:
		{
			res = pRequestString.find("\r\n", pOffset);
			if (res == std::string::npos)
				return neterr_parse_cantParse;

			if (res == pOffset)
				++emptyLines;
			
			pOffset = res + 2; // skip \r\n
		}

		return neterr_noErr;
	}

} // ns NetLib
