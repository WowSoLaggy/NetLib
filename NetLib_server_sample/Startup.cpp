#include "stdafx.h"

#include "Server.h"


using namespace NetLib;


void OnClientAccepted(std::string pClientAddress, int pClientPort)
{
	LOG("OnClientAccepted()");

	echo("Accepted: ", pClientAddress, ":", pClientPort);
}

int main(int argc, char *argv[])
{
	CreateDirectory("Logs", nullptr);
	LOGINIT("Logs\\Server.log", "NetLib Server", "NetLib_server_sample.exe");
	NETINIT;

	LOG("main()");
	NetErrCode err;

	Server server;
	err = server.Start(32167, OnClientAccepted);
	echo("Listen result: ", err);
	if (err != neterr_noErr)
		return 0;

	std::getchar();

	err = server.Stop();
	echo("Stop result: ", err);
	if (err != neterr_noErr)
		return 0;

	// Finish

	std::getchar();

	NETDISPOSE;
	LOGDISPOSE;

	return 0;
}
