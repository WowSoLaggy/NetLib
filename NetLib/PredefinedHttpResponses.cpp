#include "HttpResponse.h"

#include "Config.h"


namespace NetLib
{

	HttpResponse HttpResponse::BadRequest()
	{
		HttpResponse response;

		response.m_httpVersion = Config::GetServerHttpVersion();
		response.m_statusCode = respcode_badrequest;

		response.m_body = "<html><body><h1>400 Bad Request</h1></body></html>"; // Take this from some default page
		response.m_headers.insert({ "Content-Length", std::to_string(response.m_body.size()) });
		response.m_headers.insert({ "Content-Type", "text/html" });

		return response;
	}

	HttpResponse HttpResponse::MethodNotAllowed()
	{
		HttpResponse response;

		response.m_httpVersion = Config::GetServerHttpVersion();
		response.m_statusCode = respcode_methodnotallowed;

		response.m_body = "<html><body><h1>405 Method Not Allowed</h1></body></html>"; // Take this from some default page
		response.m_headers.insert({ "Content-Length", std::to_string(response.m_body.size()) });
		response.m_headers.insert({ "Content-Type", "text/html" });

		return response;
	}

	HttpResponse HttpResponse::MethodNotImplemented()
	{
		HttpResponse response;

		response.m_httpVersion = Config::GetServerHttpVersion();
		response.m_statusCode = respcode_methodnotimpl;

		response.m_body = "<html><body><h1>501 Method Not Implemented</h1></body></html>"; // Take this from some default page
		response.m_headers.insert({ "Content-Length", std::to_string(response.m_body.size()) });
		response.m_headers.insert({ "Content-Type", "text/html" });

		return response;
	}

	HttpResponse HttpResponse::RequestUrlTooLong()
	{
		HttpResponse response;

		response.m_httpVersion = Config::GetServerHttpVersion();
		response.m_statusCode = respcode_uritoolarge;

		response.m_body = "<html><body><h1>414 URI Too Large</h1></body></html>"; // Take this from some default page
		response.m_headers.insert({ "Content-Length", std::to_string(response.m_body.size()) });
		response.m_headers.insert({ "Content-Type", "text/html" });

		return response;
	}

} // ns NetLib
