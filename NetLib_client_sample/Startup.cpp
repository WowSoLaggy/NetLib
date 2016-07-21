#include "stdafx.h"

#include "NetLib.h"
using namespace NetLib::NetErrCodes;


void OnReceivedFromServer(char *pData, int pDataLength)
{
	LOG("OnReceivedFromServer()");
	std::string text(pData, pDataLength);
	echo("Server: \"", text.c_str(), "\"");
}


int main(int argc, char *argv[])
{
	CreateDirectory("Logs", nullptr);
	LOGINIT("Logs\\Client.log", "NetLib Client", "NetLib_client_sample.exe");
	NETINIT;

	LOG("main()");
	NetErrCode err;

	NetLib::Client client(OnReceivedFromServer);
	err = client.Connect("127.0.0.1", 32167);
	if (err != neterr_noErr)
	{
		echo("Connect error: ", err, ".");
		std::getchar();
		return 0;
	}
	echo("Connect OK.");

	echo("Enter text to send to the server. Enter 'q' to quit.");
	while (true)
	{
		std::string line;
		std::getline(std::cin, line);
		if (line.compare("q") == 0)
			break;
		
		err = client.SendToServer(line.c_str(), (int)line.size());
		if (err == neterr_noErr)
			echo("Sent ", line.size(), " bytes.");
		else
			echo("Error occurred: ", err);
	}

	err = client.Disconnect();
	if (err != neterr_noErr)
		echo("Disconnect error: ", err, ".");
	else
		echo("Disconnect OK.");
	std::getchar();

	return 0;
}
