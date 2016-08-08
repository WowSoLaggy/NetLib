#pragma once

#ifndef NETLIB_HTTPREQUEST_H
#define NETLIB_HTTPREQUEST_H


#include <vector>
#include <string>
#include <map>

#include "Utils.h"
#include "RequestMethods.h"
#include "Uri.h"
#include "HttpVersions.h"


namespace NetLib
{

	// Class that represents the Http Request
	class HttpRequest
	{
	public:

		const RequestMethod & GetMethod() const { return m_method; }			// Returns the request method
		const Uri & GetUri() const { return m_uri; }							// Returns the request Uri
		const HttpVersion & GetHttpVersion() const { return m_httpVersion; }	// Returns the request Http version

		std::map<std::string, std::string> & GetHeaders() { return m_headers; }	// Returns the map of request headers
		const std::string & GetBody() const { return m_body; }					// Returns the string that represents the request body


		// Parses the given request string
		// Params:
		// [in]  const std::string & pRequestString	- request string to parse
		// [out] int & pOffset						- number of parsed chars in the given request string
		NetErrCodes::NetErrCode Parse(const std::string &pRequestString, int &pOffset);

		std::string ToString(bool pWithBody) const;


	private:

		RequestMethod m_method;							// Request method
		Uri m_uri;										// Request Uri
		HttpVersion m_httpVersion;						// Request Http Version

		std::map<std::string, std::string> m_headers;	// Map of request headers
		std::string m_body;								// String that represents the request body

	};

} // ns NetLib


#endif // NETLIB_HTTPREQUEST_H
