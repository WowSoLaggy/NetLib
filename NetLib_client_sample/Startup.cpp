#include "stdafx.h"

#include "NetLib.h"
using namespace NetLib::NetErrCodes;


int main(int argc, char *argv[])
{
	CreateDirectory("Logs", nullptr);
	LOGINIT("Logs\\Client.log", "NetLib Client", "NetLib_client_sample.exe");
	NETINIT();

	LOG("main()");
	NetErrCode err;

	NetLib::Client client;
	err = client.Connect("127.0.0.1", 32167);
	if (err != neterr_noErr)
	{
		echo("Connect error: ", err, ".");
		std::getchar();
		return 0;
	}
	echo("Connect OK.");

	echo("Enter text to send to the server. Enter 'q' to quit. Enter 'r' to try to receive.");
	while (true)
	{
		std::string line;
		std::getline(std::cin, line);
		if (line.compare("q") == 0)
			break;
		if (line.compare("r") == 0)
		{
			std::vector<char> buffer(1024);
			int bytesReceived = 0;
			err = client.Receive(&buffer[0], (int)buffer.size(), bytesReceived);
			if (err != neterr_noErr)
				echo("Error receiving data: ", err, ".");
			else
			{
				if (bytesReceived != 0)
				{
					std::string str = std::string(buffer.data(), bytesReceived);
					echo(str);
				}
				else
					echo("Nothing to receive.");
			}
			continue;
		}

		err = client.Send(line.c_str(), (int)line.size());
		if (err == neterr_noErr)
			echo("Sent ", line.size(), " bytes.");
		else
			echo("Error occurred: ", err);
	}

	err = client.Disconnect();
	if (err != neterr_noErr)
		echo("Disconnect error: ", err);
	else
		echo("Disconnect OK");
	std::getchar();

	return 0;
}
