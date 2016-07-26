#include "stdafx.h"


using namespace NetLib;


void OnRequestFromClient(const ClientInfo &pClientInfo, const HttpRequest &pRequest)
{
}


int main(int argc, char *argv[])
{
	CreateDirectory("Logs", nullptr);
	LOGINIT("Logs\\Server.log", "NetLib Server", "NetLib_server_sample.exe");
	NETINIT;

	LOG("main()");
	NetErrCode err;

	NetLib::HttpServer serv(OnRequestFromClient);
	err = serv.Start();
	if (err != neterr_noErr)
	{
		echo("Can't start server: ", err, ".");
		echo("Press enter to exit.");
		std::getchar();
		return 1;
	}

	echo("Server started. Press enter to stop.");
	std::getchar();

	err = serv.Stop();
	if (err != neterr_noErr)
		echo("Can't stop server.");
	else
		echo("Server stopped.");

	echo("Press enter to exit.");
	std::getchar();

	return 0;
}
