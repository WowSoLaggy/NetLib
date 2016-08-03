#include "HttpResponse.h"

#include "Config.h"


namespace NetLib
{

	HttpResponse::HttpResponse()
	{
	}

	HttpResponse::~HttpResponse()
	{
	}


	std::string HttpResponse::ToString() const
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
		if (!m_body.empty())
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

} // ns NetLib
