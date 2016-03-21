#include "stdafx.h"

#include "Client.h"


using namespace NetLib;


int main(int argc, char *argv[])
{
	CreateDirectory("Logs", nullptr);
	LOGINIT("Logs\\Client.log", "NetLib Client", "NetLib_client_sample.exe");
	NETINIT;

	LOG("main()");
	NetErrCode err;

	Client client;
	err = client.Connect("127.0.0.1", 32167);
	echo("Connect result: ", err);
	if (err != neterr_noErr)
		return 0;

	std::getchar();

	err = client.Disconnect();
	echo("Disconnect result: ", err);
	if (err != neterr_noErr)
		return 0;

	// Finish

	std::getchar();

	NETDISPOSE;
	LOGDISPOSE;

	return 0;
}
