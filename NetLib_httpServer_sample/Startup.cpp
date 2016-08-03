#include "stdafx.h"


using namespace NetLib;


NetLib::HttpServer *g_server;


void OnRequestFromClient(const ClientInfo &pClientInfo, const HttpRequest &pRequest)
{
	switch (pRequest.GetMethod())
	{
	case req_get:
		g_server->SendToClient(pClientInfo.Id, NetLib::HttpResponse::MethodNotImplemented());
		break;
	case req_head:
		g_server->SendToClient(pClientInfo.Id, NetLib::HttpResponse::MethodNotImplemented());
		break;
	default:
		g_server->SendToClient(pClientInfo.Id, NetLib::HttpResponse::MethodNotImplemented());
		break;
	}
}


int main(int argc, char *argv[])
{
	CreateDirectory("Logs", nullptr);
	LOGINIT("Logs\\HttpServer.log", "NetLib Sample HttpServer", "NetLib_httpServer_sample.exe");
	NETINIT;

	LOG("main()");
	NetErrCode err;

	NetLib::HttpServer serv(OnRequestFromClient);
	g_server = &serv;
	err = serv.Start("../Config/Config.txt");
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

	return 0;
}
