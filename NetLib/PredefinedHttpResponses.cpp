#include "HttpResponse.h"

#include "Config.h"
#include "Utils.h"


namespace NetLib
{

	HttpResponse HttpResponse::BadRequest()
	{
		HttpResponse response;

		response.m_httpVersion = Config::GetServerHttpVersion();
		response.m_statusCode = respcode_badrequest;

		std::string filePath = Config::GetRootFolder() + "/" + Config::GetDefaultPage400();
		if (!CheckFileExists(filePath))
			response.m_body = "<html><head><title>400 Bad Request</title><meta charset = \"utf-8\"/></head><body><h1>400 Bad Request</h1></body></html>";
		else
			response.m_body = ReadFileToString(filePath);
		response.m_headers.insert({ "Content-Length", std::to_string(response.m_body.size()) });
		response.m_headers.insert({ "Content-Type", "text/html" });

		return response;
	}

	HttpResponse HttpResponse::MethodNotAllowed()
	{
		HttpResponse response;

		response.m_httpVersion = Config::GetServerHttpVersion();
		response.m_statusCode = respcode_methodnotallowed;

		std::string filePath = Config::GetRootFolder() + "/" + Config::GetDefaultPage405();
		if (!CheckFileExists(filePath))
			response.m_body = "<html><head><title>405 Method Not Allowed</title><meta charset = \"utf-8\"/></head><body><h1>405 Method Not Allowed</h1></body></html>";
		else
			response.m_body = ReadFileToString(filePath);
		response.m_headers.insert({ "Content-Length", std::to_string(response.m_body.size()) });
		response.m_headers.insert({ "Content-Type", "text/html" });

		return response;
	}

	HttpResponse HttpResponse::MethodNotImplemented()
	{
		HttpResponse response;

		response.m_httpVersion = Config::GetServerHttpVersion();
		response.m_statusCode = respcode_methodnotimpl;

		std::string filePath = Config::GetRootFolder() + "/" + Config::GetDefaultPage501();
		if (!CheckFileExists(filePath))
			response.m_body = "<html><head><title>501 Method Not Implemented</title><meta charset = \"utf-8\"/></head><body><h1>501 Method Not Implemented</h1></body></html>";
		else
			response.m_body = ReadFileToString(filePath);
		response.m_headers.insert({ "Content-Length", std::to_string(response.m_body.size()) });
		response.m_headers.insert({ "Content-Type", "text/html" });

		return response;
	}

	HttpResponse HttpResponse::RequestUrlTooLong()
	{
		HttpResponse response;

		response.m_httpVersion = Config::GetServerHttpVersion();
		response.m_statusCode = respcode_uritoolarge;

		std::string filePath = Config::GetRootFolder() + "/" + Config::GetDefaultPage414();
		if (!CheckFileExists(filePath))
			response.m_body = "<html><head><title>414 URI Too Large</title><meta charset = \"utf-8\"/></head><body><h1>414 URI Too Large</h1></body></html>";
		else
			response.m_body = ReadFileToString(filePath);
		response.m_headers.insert({ "Content-Length", std::to_string(response.m_body.size()) });
		response.m_headers.insert({ "Content-Type", "text/html" });

		return response;
	}

	HttpResponse HttpResponse::FileNotFound()
	{
		HttpResponse response;

		response.m_httpVersion = Config::GetServerHttpVersion();
		response.m_statusCode = respcode_notfound;

		std::string filePath = Config::GetRootFolder() + "/" + Config::GetDefaultPage404();
		if (!CheckFileExists(filePath))
			response.m_body = "<html><head><title>404 Not Found</title><meta charset = \"utf-8\"/></head><body><h1>404 Not Found</h1></body></html>";
		else
			response.m_body = ReadFileToString(filePath);
		response.m_headers.insert({ "Content-Length", std::to_string(response.m_body.size()) });
		response.m_headers.insert({ "Content-Type", "text/html" });

		return response;
	}

} // ns NetLib
