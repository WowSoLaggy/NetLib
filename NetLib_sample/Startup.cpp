#include "stdafx.h"

#include "Server.h"


int main(int argc, char *argv[])
{
	NetLib::Server server;
	server.Start();

    return 0;
}
