#include "stdafx.h"


using namespace NetLib;

CLIENTID lastClient = 0;


void OnClientAccepted(const ClientInfo &pClientInfo)
{
	LOG("OnClientAccepted()");
	lastClient = pClientInfo.Id;
	echo("Accepted id: ", pClientInfo.Id, " from ", pClientInfo.Address, ":", pClientInfo.Port);
}
void OnClientDisconnected(const ClientInfo &pClientInfo)
{
	LOG("OnClientDisconnected()");
	echo("Disconnected id: ", pClientInfo.Id);
}
void OnReceivedFromClient(const ClientInfo &pClientInfo, char *pData, int pDataLength)
{
	LOG("OnReceivedFromClient()");
	std::string text(pData, pDataLength);
	echo(pClientInfo.Id, ": \"", text.c_str(), "\"");
}


int main(int argc, char *argv[])
{
	CreateDirectory("Logs", nullptr);
	LOGINIT("Logs\\Server.log", "NetLib Server", "NetLib_server_sample.exe");
	NETINIT;

	LOG("main()");
	NetErrCode err;

	Server server(OnClientAccepted, OnClientDisconnected, OnReceivedFromClient);
	err = server.Start(32167);
	if (err != neterr_noErr)
	{
		echo("Server start error: ", err, ".");
		std::getchar();
		return 0;
	}
	echo("Server started OK.");

	echo("Enter text to send to the last connected client. Enter 'q' to quit.");
	while (true)
	{
		std::string line;
		std::getline(std::cin, line);
		if (line.compare("q") == 0)
			break;

		err = server.SendToClient(lastClient, line.c_str(), (int)line.size());
		if (err == neterr_noErr)
			echo("Sent ", line.size(), " bytes.");
		else
			echo("Error occurred: ", err, ".");
	}

	err = server.Stop();
	if (err != neterr_noErr)
		echo("Server stop error: ", err, ".");
	else
		echo("Server stopped OK.");
	std::getchar();

	return 0;
}
