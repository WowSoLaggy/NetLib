#include "stdafx.h"

#include "Server.h"


using namespace NetLib;


void OnClientAccepted(std::string pClientAddress, int pClientPort)
{
	LOG("OnClientAccepted()");

	echo("Accepted someone!");
}

int main(int argc, char *argv[])
{
	CreateDirectory("Logs", nullptr);
	LOGINIT("Logs\\NetLib.log", "NetLib", "NetLib_sample.exe");
	LOG("main()");
	NetErrCode err;

	Server server;
	err = server.Start(32167, OnClientAccepted);
	echo(std::string("Listen result: ").append(std::to_string(err)));
	if (err != neterr_noErr)
		return 0;

	std::getchar();

	err = server.Stop();
	echo(std::string("Stop result: ").append(std::to_string(err)));
	if (err != neterr_noErr)
		return 0;

	// Finish

	std::getchar();

	LOGDISPOSE;

    return 0;
}
