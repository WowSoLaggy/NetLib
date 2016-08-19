#include "HttpResponse.h"

#include <sstream>
#include <algorithm>
#include "Utils.h"
#include "Log.h"
#include "Config.h"


namespace NetLib
{

	HttpResponse::HttpResponse()
	{
	}

	HttpResponse::~HttpResponse()
	{
	}


	std::string HttpResponse::ToString(bool pWithBody) const
	{
		std::string res = "";

		// Response status line
		res.append(g_httpVersionsMap[m_httpVersion]).
			append(" ").
			append(std::to_string(std::get<0>(g_responseStatusMap[m_statusCode]))).
			append(" ").
			append(std::get<1>(g_responseStatusMap[m_statusCode])).
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

	void HttpResponse::AddHeader(const std::string &pHeaderName, const std::string &pHeaderValue)
	{
		m_headers.insert({ pHeaderName, pHeaderValue });
	}

	void HttpResponse::AddBody(const std::string &pBodyString)
	{
		m_body.append(pBodyString);
	}

	NetErrCode HttpResponse::Parse(const std::string &pResponseString, int &pOffset)
	{
		LOG("HttpRequest::Parse()");

		std::string str;
		std::vector<std::string> tokens;
		pOffset = 0;

		// Remove all \r from the response string

		std::string responseString = pResponseString;
		auto it = std::remove(responseString.begin(), responseString.end(), '\r');
		int removedChars = std::distance(it, responseString.end());
		responseString.erase(it, responseString.end());


		std::istringstream lines(responseString);
		std::string line;

		// Get Response Line
		if (!std::getline(lines, line))
		{
			// Can't get even the first line, lol
			echo("ERROR: Can't get the Response Line.");
			return neterr_parse_cantParse;
		}

		// Parse Response Line

		tokens = SplitString(line, ' ');
		if (tokens.size() < 3)
		{
			// Strange Request Line format, it should be:
			// HTTPVER CODE PHRASE
			echo("ERROR: Request Line invalid format: \"", line, "\".");
			return neterr_parse_cantParse;
		}

		// Response Http version

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
				echo("ERROR: Can't recognize the HTTP version: \"", tokens[0], "\".");
				return neterr_parse_cantParse;
			}

			if (tokens[0].compare(version.second) == 0)
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

		// Response status code

		for (auto & status : g_responseStatusMap)
		{
			if (status.first == respcode_unknown)
			{
				// Skip the first element of the enumeration
				continue;
			}
			if (status.first == respcode_end)
			{
				// It is already the last element of the enumeration and
				// we still don't have any match
				echo("ERROR: Can't recognize the Response Status Code: \"", tokens[1], "\".");
				return neterr_parse_cantParse;
			}

			if (tokens[1].compare(std::to_string(std::get<int>(status.second))) == 0)
			{
				m_statusCode = status.first;
				break;
			}
		}

		// Response reason-phrase

		m_reasonPhrase = tokens[2];
		for (int i = 3; i < (int)tokens.size(); ++i)
			m_reasonPhrase = m_reasonPhrase.append(" ").append(tokens[i]);

		// Response headers

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
			if (contentLength > Config::GetResponseBodyMaxLength())
			{
				// No need to log because it's not an error
				return neterr_parse_bodyTooLong;
			}

			// Response body

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

			if ((int)m_body.size() > Config::GetResponseBodyMaxLength())
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

} // ns NetLib
