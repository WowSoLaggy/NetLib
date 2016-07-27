#include "HttpResponse.h"


namespace NetLib
{

	HttpResponse HttpResponse::BadRequest(bool pBody)
	{
		HttpResponse response;

		response.m_httpVersion = httpver_1_0; // TODO: move to config
		response.m_statusCode = respcode_badrequest;

		if (pBody)
		{
			response.m_body = "<html><body><h1>400 Bad Request</h1></body></html>"; // Take this from some default page
			response.m_headers.insert({ "Content-Length", std::to_string(response.m_body.size()) });
			response.m_headers.insert({ "Content-Type", "text/html" });
		}

		return response;
	}

	HttpResponse HttpResponse::MethodNotImplemented(bool pBody)
	{
		HttpResponse response;

		response.m_httpVersion = httpver_1_0; // TODO: move to config
		response.m_statusCode = respcode_methodnotimpl;

		if (pBody)
		{
			response.m_body = "<html><body><h1>501 Method Not Implemented</h1></body></html>"; // Take this from some default page
			response.m_headers.insert({ "Content-Length", std::to_string(response.m_body.size()) });
			response.m_headers.insert({ "Content-Type", "text/html" });
		}

		return response;
	}

	HttpResponse HttpResponse::RequestUrlTooLong(bool pBody)
	{
		HttpResponse response;

		response.m_httpVersion = httpver_1_0; // TODO: move to config
		response.m_statusCode = respcode_uritoolarge;

		if (pBody)
		{
			response.m_body = "<html><body><h1>414 URI Too Large</h1></body></html>"; // Take this from some default page
			response.m_headers.insert({ "Content-Length", std::to_string(response.m_body.size()) });
			response.m_headers.insert({ "Content-Type", "text/html" });
		}

		return response;
	}

} // ns NetLib
