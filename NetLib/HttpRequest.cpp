#include "HttpRequest.h"

#include <sstream>
#include <iostream>
#include <algorithm>

#include "Log.h"
#include "Config.h"


namespace NetLib
{
	using namespace NetErrCodes;


	NetErrCode HttpRequest::Parse(const std::string &pRequestString, int &pOffset)
	{
		LOG("HttpRequest::Parse()");

		std::string str;
		std::vector<std::string> tokens;
		pOffset = 0;

		// Remove all \r from the request string

		std::string requestString = pRequestString;
		auto it = std::remove(requestString.begin(), requestString.end(), '\r');
		int removedChars = std::distance(it, requestString.end());
		requestString.erase(it, requestString.end());


		std::istringstream lines(requestString);
		std::string line;
		
		// Get Request Line
		if (!std::getline(lines, line))
		{
			// Can't get even the first line, lol
			echo("ERROR: Can't get the Request Line.");
			return neterr_parse_cantParse;
		}
		if ((int)line.size() > Config::GetRequestUriMaxLength() + 17) // +17 due to method name and http version in this line
		{
			// The Request Line is too long (URI or whatever, it should not be SO long)
			// We can log it if it is twice as long as possible. If it is more, then no log

			if ((int)line.size() < Config::GetRequestUriMaxLength() * 2 + 17)
				echo("ERROR: The Request Line is too long: \"", line, "\".");
			else
				echo("ERROR: The Request Line is too long.");

			return neterr_parse_requestLineTooLong;
		}

		// Parse request line

		tokens = SplitString(line, ' ');
		if (tokens.size() != 3)
		{
			// Strange Request Line format, it should be:
			// METHOD URI HTTPVER
			echo("ERROR: Request Line invalid format: \"", line, "\".");
			return neterr_parse_cantParse;
		}

		// Request method

		for (auto & method : g_requestMethodsMap)
		{
			if (method.first == req_unknown)
			{
				// Skip the first element of the enumeration
				continue;
			}
			if (method.first == req_end)
			{
				// It is already the last element of the enumeration and
				// we still don't have any match
				echo("ERROR: Can't recognize the Request Method: \"", tokens[0], "\".");
				return neterr_parse_cantParse;
			}

			if (tokens[0].compare(method.second) == 0)
			{
				m_method = method.first;

				// Check whether the request method is allowed
				auto allowedMethods = Config::GetAllowedRequestMethods();
				if (std::find(allowedMethods.begin(), allowedMethods.end(), method.second) == allowedMethods.end())
				{
					// Nope. Not this time.
					// No need to log, because it's not an error
					return neterr_parse_methodNotAllowed;
				}

				break;
			}
		}

		// Request Uri

		m_uri = Uri(tokens[1]);
		if ((int)m_uri.ToString().size() > Config::GetRequestUriMaxLength())
		{
			// URI is too long. We can log it if it is twice as long as possible. If it is more, then no log

			if ((int)m_uri.ToString().size() < Config::GetRequestUriMaxLength() * 2)
				echo("ERROR: The Request URI is too long: \"", m_uri.ToString(), "\".");
			else
				echo("ERROR: The Request URI is too long.");

			return neterr_parse_requestLineTooLong;
		}

		// Request Http version

		for (auto & version : g_httpVersionsMap)
		{
			if (version.first == httpver_unknown)
			{
				// Skip the first element of the enumeration
				continue;
			}
			if (version.first == httpver_end)
			{
				// It is already the last element of the enumeration and
				// we still don't have any match
				echo("ERROR: Can't recognize the HTTP version: \"", tokens[2], "\".");
				return neterr_parse_cantParse;
			}

			if (tokens[2].compare(version.second) == 0)
			{
				m_httpVersion = version.first;

				// Check whether the http version of the request is supported
				auto supportedHttpVersions = Config::GetSupportedHttpVersions();
				if (std::find(supportedHttpVersions.begin(), supportedHttpVersions.end(), m_httpVersion) == supportedHttpVersions.end())
				{
					// HTTP version is not supported.
					// No need to log because it's not an error
					return neterr_parse_cantParse;
				}

				break;
			}
		}

		// Request headers

		while (std::getline(lines, line))
		{
			if (line.empty())
			{
				// End of headers
				break;
			}

			tokens = SplitString(line, ':');
			if (tokens.size() < 2)
			{
				// Invalid header syntax, can't find the ':' symbol
				echo("ERROR: Invalid header syntax for the line: \"", line, "\".");
				return neterr_parse_cantParse;
			}

			std::string headerName = tokens[0];
			line.erase(0, tokens[0].size() + 1); // +1 to delete ':'
			line = TrimString(line);
			m_headers.insert({ headerName, line });
		}

		// Check whether there should be a content

		std::string contentLengthString = m_headers["Content-length"];
		if (!contentLengthString.empty())
		{
			int contentLength = std::stoi(contentLengthString);
			if (contentLength > Config::GetRequestBodyMaxLength())
			{
				// No need to log because it's not an error
				return neterr_parse_bodyTooLong;
			}

			// Request body

			m_body = "";
			while (std::getline(lines, line))
			{
				if (line.empty())
				{
					// This should be the end of the body
					break;
				}

				// Just append, no need to parse
				m_body.append(line).append("\n");
			}

			if ((int)m_body.size() > Config::GetRequestBodyMaxLength())
			{
				// No need to log because it's not an error
				return neterr_parse_bodyTooLong;
			}

			if (m_body.size() != contentLength)
			{
				// Actual body length does not match the declared
				echo("ERROR: Actual body length (", m_body.size(), ") does not match the declared (", contentLength, ").");
				return neterr_parse_cantParse;
			}
		}

		pOffset = (int)lines.tellg() + removedChars;

		return neterr_noErr;
	}

	std::string HttpRequest::ToString(bool pWithBody) const
	{
		std::string res = "";

		// Request status line
		res.append(g_requestMethodsMap[m_method]).
			append(" ").
			append(m_uri.ToString()).
			append(" ").
			append(g_httpVersionsMap[m_httpVersion]).
			append(Config::GetHttpNewLine());

		// Headers
		for (auto & header : m_headers)
			res.append(header.first).append(": ").append(header.second).append(Config::GetHttpNewLine());
		res.append(Config::GetHttpNewLine());

		// Body if any
		if ((!m_body.empty()) && (pWithBody))
			res.append(m_body).append(Config::GetHttpNewLine()).append(Config::GetHttpNewLine());

		return res;
	}

} // ns NetLib
