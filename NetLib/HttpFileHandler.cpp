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


		if ((pHttpRequest.GetMethod() == req_get) || (pHttpRequest.GetMethod() == req_head))
		{
			// GET or HEAD

			p = p.append(pHttpRequest.GetUri().ToString());
			if (is_directory(p))
				p = p.append(Config::GetDefaultIndex());

			HttpResponse response;

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

			err = SendToClient(pClientInfo.Id, response);
			if (err != neterr_noErr)
			{
				echo("ERROR: Can't send response to the client (id: ", pClientInfo.Id, ").");
				return err;
			}
		}
		else if (pHttpRequest.GetMethod() == req_put)
		{
			// PUT

			err = SendToClient(pClientInfo.Id, NetLib::HttpResponse::MethodNotImplemented());
			if (err != neterr_noErr)
			{
				echo("ERROR: Can't send response to the client (id: ", pClientInfo.Id, ").");
				return err;
			}
		}
		else if (pHttpRequest.GetMethod() == req_delete)
		{
			// DELETE

			err = SendToClient(pClientInfo.Id, NetLib::HttpResponse::MethodNotImplemented());
			if (err != neterr_noErr)
			{
				echo("ERROR: Can't send response to the client (id: ", pClientInfo.Id, ").");
				return err;
			}
		}
		else
		{
			echo("ERROR: Can't recognize method: \"", pHttpRequest.GetMethod(), "\".");
			return neterr_fileHandlerMethodNotRecognized;
		}

		return neterr_noErr;
	}

} // ns NetLib


//#include <string>
//#include <iostream>
//#include <iomanip>
//#include <filesystem>
//#include <chrono>
//#include <time.h>
//
//using namespace std;
//using namespace std::experimental::filesystem::v1;
//
//// Display the last write time for the file
//wstring LastWriteTimeToLocalTime(const path& file_path)
//{
//	const auto last = chrono::system_clock::to_time_t(last_write_time(file_path));
//	tm timeinfo; localtime_s(&timeinfo, &last);
//	wchar_t buf[56]; _wasctime_s(buf, 56, &timeinfo); // appends '\n'
//	return wstring{ buf };
//}
//
//// List files and directories in the specified path
//void DisplayFolderContents(const path& p)
//{
//	wcout << L"Begin iterating " << p.wstring() << endl;
//	for (const auto& entry : directory_iterator{ p })
//	{
//		if (is_regular_file(entry.status()))
//		{
//			wcout << L" File: " << entry.path().filename() << " : " << LastWriteTimeToLocalTime(entry.path());
//		}
//		else if (is_directory(entry.status()))
//		{
//			wcout << L" Dir: " << entry.path().filename() << endl;
//		}
//	}
//}
//
//void main()
//{
//	wstring dir{ LR"(C:\users\public\documents\)" };
//	path p{ dir };
//	if (!is_directory(p))
//	{
//		wcout << L"No such directory: " << dir << endl;
//		return;
//	}
//	DisplayFolderContents(p);
//	// IterateFolderRecursively(p);	// see example
//	wcout << endl << L"Press Enter to exit" << endl;
//	wstring input;
//	getline(wcin, input);
//}
//
//// List files and directories recursively in the path
//void IterateFolderRecursively(const path& p)
//{
//	wcout << L"Begin iterating " << p.wstring() << " recursively" << endl;
//	for (recursive_directory_iterator it{ p }, end; it != end; ++it)
//	{
//		if (is_regular_file(it->status()))
//		{
//			wcout << setw(it.depth()) << L" " << L"File: " << it->path().filename() << L" : " << LastWriteTimeToLocalTime(it->path());
//		}
//		else if (is_directory(it->status()))
//		{
//			wcout << setw(it.depth()) << L" " << L"Dir: " << it->path().filename() << endl;
//		}
//	}
//}
