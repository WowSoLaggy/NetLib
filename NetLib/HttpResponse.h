#pragma once

#ifndef NETLIB_HTTPRESPONSE_H
#define NETLIB_HTTPRESPONSE_H


#include <string>

#include "HttpVersions.h"
#include "ResponseStatusCodes.h"


namespace NetLib
{

	// Class that represents the Http response
	class HttpResponse
	{
	public:


		// Static predefined responses


		// Returns the default Bad Request response
		static HttpResponse BadRequest();

		// Returns the default Method Not Allowed response
		static HttpResponse MethodNotAllowed();

		// Returns the default Method Not Implemented response
		static HttpResponse MethodNotImplemented();

		// Returns the default Request Url Too Long response
		static HttpResponse RequestUrlTooLong();

		// Returns the default File Not Found response
		static HttpResponse FileNotFound();


	public:

		HttpResponse();
		virtual ~HttpResponse();

		// Returns the response converted to the string
		std::string ToString() const;

		// Adds the given header to the response
		void AddHeader(const std::string &pHeaderName, const std::string &pHeaderValue);

	private:

		HttpVersion m_httpVersion;						// Response Http version
		ResponseStatusCode m_statusCode;				// Response status code

		std::map<std::string, std::string> m_headers;	// Map of response headers
		std::string m_body;								// Response body

	};

} // NetLib


#endif // NETLIB_HTTPRESPONSE_H
