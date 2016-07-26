#pragma once

#ifndef NETLIB_HTTPRESPONSE_H
#define NETLIB_HTTPRESPONSE_H


#include "HttpVersions.h"
#include "ResponseStatusCodes.h"


namespace NetLib
{

	class HttpResponse
	{
	public:


		// Static predefined responses

		static HttpResponse BadRequest();
		static HttpResponse MethodNotAllowed();


	public:

		HttpResponse();
		virtual ~HttpResponse();

	private:

		HttpVersion m_httpVersion;
		ResponseStatusCode m_statusCode;

	};

} // NetLib


#endif // NETLIB_HTTPRESPONSE_H
