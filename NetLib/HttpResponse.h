#pragma once

#ifndef NETLIB_HTTPRESPONSE_H
#define NETLIB_HTTPRESPONSE_H


#include <string>

#include "HttpVersions.h"
#include "ResponseStatusCodes.h"


namespace NetLib
{

	class HttpResponse
	{
	public:


		// Static predefined responses

		static HttpResponse BadRequest(bool pBody);
		static HttpResponse MethodNotImplemented(bool pBody);
		static HttpResponse RequestUrlTooLong(bool pBody);


	public:

		HttpResponse();
		virtual ~HttpResponse();

		std::string ToString() const;

	private:

		HttpVersion m_httpVersion;
		ResponseStatusCode m_statusCode;

		std::map<std::string, std::string> m_headers;
		std::string m_body;

	};

} // NetLib


#endif // NETLIB_HTTPRESPONSE_H
