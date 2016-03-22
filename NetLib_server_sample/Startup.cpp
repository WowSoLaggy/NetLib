#include "stdafx.h"

#include "Server.h"


using namespace NetLib;


void OnClientAccepted(unsigned int pClientId, std::string pClientAddress, int pClientPort)
{
	LOG("OnClientAccepted()");
	echo("Accepted id: ", pClientId, " from ", pClientAddress, ":", pClientPort);
}
void OnClientDisconnected(unsigned int pClientId)
{
	LOG("OnClientDisconnected()");
	echo("Disconnected id: ", pClientId);
}
void OnClientDataReceived(unsigned int pClientId, char *pData, int pDataLength)
{
	LOG("OnClientDataReceived()");
	std::string text(pData, pDataLength);
	echo(pClientId, ": \"", text.c_str(), "\"");
}

int main(int argc, char *argv[])
{
	CreateDirectory("Logs", nullptr);
	LOGINIT("Logs\\Server.log", "NetLib Server", "NetLib_server_sample.exe");
	NETINIT;

	LOG("main()");
	NetErrCode err;

	Server server;
	err = server.Start(32167, OnClientAccepted, OnClientDisconnected, OnClientDataReceived);
	echo("Listen result: ", err);
	std::getchar();
	if (err != neterr_noErr)
		return 0;

	err = server.Stop();
	echo("Stop result: ", err);
	std::getchar();
	if (err != neterr_noErr)
		return 0;

	// Finish

	NETDISPOSE;
	LOGDISPOSE;

	return 0;
}
