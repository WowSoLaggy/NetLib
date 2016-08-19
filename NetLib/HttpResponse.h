#pragma once

#ifndef NETLIB_HTTPRESPONSE_H
#define NETLIB_HTTPRESPONSE_H


#include <string>

#include "NetErrCodes.h"
#include "HttpVersions.h"
#include "ResponseStatusCodes.h"


namespace NetLib
{
	using namespace NetErrCodes;


	// Class that represents the Http response
	class HttpResponse
	{
	public:


		// Static predefined responses


		// Returns the default OK response
		static HttpResponse Ok();

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

		// Returns the default AuthRequired response
		static HttpResponse AuthRequired();

		// Returns the default Forbidden response
		static HttpResponse Forbidden();

		// Returns the default Internal Server Error response
		static HttpResponse InternalServerError();


	public:

		HttpResponse();
		virtual ~HttpResponse();

		// Returns the response converted to the string
		std::string ToString(bool pWithBody) const;

		// Adds the given header to the response
		void AddHeader(const std::string &pHeaderName, const std::string &pHeaderValue);

		// Adds the given string to the body
		void AddBody(const std::string &pBodyString);

		// Returns the length of the body
		int GetBodySize() { return m_body.size(); }

		// Parses the given response string
		// Params:
		// [in]  const std::string & pResponseString	- response string to parse
		// [out] int & pOffset							- number of parsed chars in the given response string
		NetErrCode Parse(const std::string &pResponseString, int &pOffset);

	private:

		HttpVersion m_httpVersion;						// Response Http version
		ResponseStatusCode m_statusCode;				// Response status code
		std::string m_reasonPhrase;						// Response reason phrase

		std::map<std::string, std::string> m_headers;	// Map of response headers
		std::string m_body;								// Response body

	};

} // NetLib


#endif // NETLIB_HTTPRESPONSE_H
