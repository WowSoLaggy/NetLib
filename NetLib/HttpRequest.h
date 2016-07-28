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

	class HttpRequest
	{
	public:

		const RequestMethod & GetMethod() const { return m_method; }
		const Uri & GetUri() const { return m_uri; }
		const HttpVersion & GetHttpVersion() const { return m_httpVersion; }

		std::map<std::string, std::string> & GetHeaders() { return m_headers; }
		const std::string & GetBody() const { return m_body; }


		NetErrCodes::NetErrCode Parse(const std::string &pRequestString, int &pOffset);


	private:

		RequestMethod m_method;
		Uri m_uri;
		HttpVersion m_httpVersion;

		std::map<std::string, std::string> m_headers;
		std::string m_body;

	};

} // ns NetLib


#endif // NETLIB_HTTPREQUEST_H
