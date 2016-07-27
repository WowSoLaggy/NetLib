#include "HttpRequest.h"

#include <sstream>
#include <iostream>
#include <algorithm>


namespace NetLib
{
	using namespace NetErrCodes;


	NetErrCode HttpRequest::Parse(const std::string &pRequestString, int &pOffset)
	{
		std::string str;
		std::vector<std::string> tokens;
		pOffset = 0;

		// Remove all \r from request string

		std::string requestString = pRequestString;
		requestString.erase(std::remove(requestString.begin(), requestString.end(), '\r'), requestString.end());


		std::istringstream lines(requestString);
		std::string line;
		
		// Get request line
		if (!std::getline(lines, line))
			return neterr_parse_cantParse;
		if (line.size() > 255) // TODO: move 255 to config
			return neterr_parse_requestLineTooLong;

		// Parse request line

		tokens = SplitString(line, ' ');
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
		if ((m_httpVersion != httpver_1_0) && (m_httpVersion != httpver_1_1)) // TODO: move supported http version to config
			return neterr_parse_cantParse;

		// Request headers

		while (std::getline(lines, line))
		{
			if (line.empty())
				break;

			tokens = SplitString(line, ':');
			if (tokens.size() < 2)
			{
				// Error header parsing
				return neterr_parse_cantParse;
			}

			std::string headerName = tokens[0];
			line.erase(0, tokens[0].size() + 1); // +1 to delete ':'
			TrimString(line);
			m_headers.insert({ headerName, line });
		}

		// Request body

		m_body = "";
		while (std::getline(lines, line))
		{
			if (line.empty())
				break;
			
			m_body.append(line).append("\n");
		}

		pOffset = (int)lines.tellg();

		return neterr_noErr;
	}

} // ns NetLib
