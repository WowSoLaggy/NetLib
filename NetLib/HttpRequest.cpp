#include "HttpRequest.h"

#include <sstream>
#include <iostream>
#include <algorithm>

#include "Config.h"


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
		auto it = std::remove(requestString.begin(), requestString.end(), '\r');
		int removedChars = std::distance(it, requestString.end());
		requestString.erase(it, requestString.end());


		std::istringstream lines(requestString);
		std::string line;
		
		// Get request line
		if (!std::getline(lines, line))
			return neterr_parse_cantParse;
		if ((int)line.size() > Config::GetRequestUriMaxLength() + 17) // +17 due to nethod name and http version in this line
			return neterr_parse_requestLineTooLong;

		// Parse request line

		tokens = SplitString(line, ' ');
		if (tokens.size() != 3)
			return neterr_parse_cantParse;

		// Request method

		for (auto & method : g_requestMethodsMap)
		{
			if (method.first == req_unknown)
				continue;

			if (tokens[0].compare(method.second) == 0)
			{
				m_method = method.first;

				// Check whether the request method is allowed
				auto allowedMethods = Config::GetAllowedRequestMethods();
				if (std::find(allowedMethods.begin(), allowedMethods.end(), method.second) == allowedMethods.end())
					return neterr_parse_methodNotAllowed;

				break;
			}
			if (method.first == req_end)
				return neterr_parse_cantParse;
		}

		// Request Uri

		m_uri = Uri(tokens[1]);
		if ((int)m_uri.ToString().size() > Config::GetRequestUriMaxLength())
			return neterr_parse_requestLineTooLong;

		// Request Http version

		for (auto & version : g_httpVersionsMap)
		{
			if (tokens[2].compare(version.second) == 0)
			{
				m_httpVersion = version.first;

				// Check whether the http version of the request is supported
				auto supportedHttpVersions = Config::GetSupportedHttpVersions();
				if (std::find(supportedHttpVersions.begin(), supportedHttpVersions.end(), m_httpVersion) == supportedHttpVersions.end())
					return neterr_parse_cantParse;

				break;
			}
			if (version.first == httpver_end)
				return neterr_parse_cantParse;
		}

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

		// Check whether there should be a content

		std::string contentLengthString = m_headers["Content-length"];
		if (!contentLengthString.empty())
		{
			int contentLength = std::stoi(contentLengthString);

			// Request body

			m_body = "";
			while (std::getline(lines, line))
			{
				if (line.empty())
					break;

				m_body.append(line).append("\n");
			}

			if (m_body.size() != contentLength)
				return neterr_parse_cantParse;
		}

		pOffset = (int)lines.tellg() + removedChars;

		return neterr_noErr;
	}

} // ns NetLib
