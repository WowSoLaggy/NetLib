#include "HttpServer.h"

#include <filesystem>

#include "Log.h"
#include "Utils.h"


namespace NetLib
{
	using namespace std::experimental::filesystem::v1;

	NetErrCode HttpServer::FileHandler(const ClientInfo &pClientInfo, const HttpRequest &pHttpRequest)
	{
		LOG("HttpServer::FileHandler()");
		NetErrCode err;

		path p(Config::GetRootFolder());
		if (!is_directory(p))
		{
			echo("ERROR: Can't find the root directory.");
			return neterr_noRootDir;
		}

		// Answer to the client
		HttpResponse response;

		switch (pHttpRequest.GetMethod())
		{
		case req_get:
		case req_head:
		{
			// GET or HEAD

			p = p.append(pHttpRequest.GetUri().ToString());
			if (is_directory(p))
			{
				// If points to a directory add something like "index.html" from the config
				p = p.append(Config::GetDefaultIndex());
			}

			if (is_regular_file(p))
			{
				response = HttpResponse::Ok();
				std::string body = ReadFileToString(p.string());
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
		err = SendToClient(pClientInfo.Id, response);
		if (err != neterr_noErr)
		{
			echo("ERROR: Can't send response to the client (id: ", pClientInfo.Id, ").");
			return err;
		}


		return neterr_noErr;
	}

} // ns NetLib
