#include "HttpResponse.h"


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
			append("\r\n");	// TODO: take these symbols from somewhere (config or what?)

		// Headers
		for (auto & header : m_headers)
			res.append(header.first).append(": ").append(header.second).append("\r\n");
		res.append("\r\n");

		if (!m_body.empty())
			res.append(m_body).append("\r\n\r\n");

		return res;
	}

} // ns NetLib
