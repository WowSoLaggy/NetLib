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
	{
		std::getchar();
		return 0;
	}

	echo("Enter text to send to the server. Enter 'q' to quit.");
	while (true)
	{
		std::string line;
		std::getline(std::cin, line);
		if (line.compare("q") == 0)
			break;

		err = client.Send(line.c_str(), line.size());
		echo("Sent ", line.size(), " bytes. Result: ", err);
	}

	err = client.Disconnect();
	echo("Disconnect result: ", err);
	std::getchar();
	if (err != neterr_noErr)
		return 0;

	// Finish

	NETDISPOSE;
	LOGDISPOSE;

	return 0;
}
