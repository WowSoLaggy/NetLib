#include "HttpServer.h"

#include <filesystem>

#include "Log.h"
#include "Utils.h"


namespace NetLib
{
	using namespace std::experimental::filesystem::v1;


	NetErrCode HttpServer::FileHandler(const HttpConnectionInfo &pHttpConnectionInfo, const HttpRequest &pHttpRequest)
	{
		LOG("HttpServer::FileHandler()");
		NetErrCode err;

		// Server root dir

		path rootPath(Config::GetRootFolder());
		if (!is_directory(rootPath))
		{
			echo("ERROR: Can't find the root directory.");
			return neterr_noRootDir;
		}

		// Resource path

		path resPath = rootPath.append(pHttpRequest.GetUri().ToString());

		HttpResponse response;

		switch (pHttpRequest.GetMethod())
		{
		case req_get:
		case req_head:
		{
			// GET or HEAD

			if (is_directory(resPath))
			{
				// If points to a directory add something like "index.html" from the config
				resPath = resPath.append(Config::GetDefaultIndex());
			}

			std::string authHeader = "";
			err = m_authorizer.CanAccess(pHttpConnectionInfo, resPath, access_read, authHeader);
			if (err == neterr_notFound)
			{
				response = HttpResponse::FileNotFound();
				break;
			}
			else if (err == neterr_authRequired)
			{
				response = HttpResponse::AuthRequired();
				if (!authHeader.empty())
					response.AddHeader("WWW-Authenticate", authHeader);
				break;
			}
			else if (err == neterr_forbidden)
			{
				response = HttpResponse::Forbidden();
				break;
			}
			else if (err != neterr_noErr)
			{
				echo("ERROR: Internal authorization error.");
				return err;
			}

			if (is_regular_file(resPath))
			{
				response = HttpResponse::Ok();
				std::string body = ReadFileToString(resPath.string());
				if (pHttpRequest.GetMethod() == req_get)
				{
					// Add body only if GET, not HEAD
					response.AddBody(body);
				}
				response.AddHeader("Content-Length", std::to_string(body.size()));
				response.AddHeader("Content-Type", "text/html");
			}
			else
				response = HttpResponse::FileNotFound();

			break;
		}
		case req_put:
		{
			// PUT

			response = NetLib::HttpResponse::MethodNotImplemented();
			break;
		}
		case req_delete:
		{
			// DELETE

			response = NetLib::HttpResponse::MethodNotImplemented();
			break;
		}
		default:
		{
			echo("ERROR: Can't recognize method: \"", pHttpRequest.GetMethod(), "\".");
			return neterr_fileHandlerMethodNotRecognized;
		}
		}


		// Send the response
		err = SendToClient(pHttpConnectionInfo.Id, response);
		if (err != neterr_noErr)
		{
			echo("ERROR: Can't send response to the client (id: ", pHttpConnectionInfo.Id, ").");
			return err;
		}


		return neterr_noErr;
	}

} // ns NetLib
