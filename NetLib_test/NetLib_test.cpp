#include "stdafx.h"
#include "CppUnitTest.h"


#include "Server.h"
#include "Client.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace NetLib_test
{
	volatile bool receivedSomething;
	std::string receivedText;
	void OnClientDataReceived(unsigned int pClientId, char *pData, int pDataLength)
	{
		receivedSomething = true;
		receivedText = std::string(pData, pDataLength);
	}
	unsigned int clientId;
	void OnClientAccepted(unsigned int pClientId, std::string pClientAddress, int pClientPort)
	{
		clientId = pClientId;
	}

	TEST_CLASS(NetLib_test)
	{
	public:

		TEST_METHOD(NetLibInitDispose)
		{
			Logger::WriteMessage("--- START ---");
			NetLib::NetErrCode err;

			err = NetLib::Net::Init();
			Assert::IsTrue(err == NetLib::NetErrCode::neterr_noErr, L"Error initializing NetLib.");
			Logger::WriteMessage("NetLib is initialized.");

			err = NetLib::Net::Dispose();
			Assert::IsTrue(err == NetLib::NetErrCode::neterr_noErr, L"Error disposing NetLib.");
			Logger::WriteMessage("NetLib is disposed.");

			Logger::WriteMessage("--- FINISH ---");
		}


		TEST_METHOD(ServerCreateDelete)
		{
			Logger::WriteMessage("--- START ---");
			NetLib::NetErrCode err;

			err = NetLib::Net::Init();
			Assert::IsTrue(err == NetLib::NetErrCode::neterr_noErr, L"Error initializing NetLib.");
			Logger::WriteMessage("NetLib is initialized.");


			NetLib::Server server(nullptr, nullptr, nullptr);
			Logger::WriteMessage("Server is initialized.");


			err = server.Stop();
			Assert::IsTrue(err == NetLib::NetErrCode::neterr_noErr, L"Error stopping server.");


			err = server.Start(32167);
			Assert::IsTrue(err == NetLib::NetErrCode::neterr_noErr, L"Error restarting server.");

			err = server.Stop();
			Assert::IsTrue(err == NetLib::NetErrCode::neterr_noErr, L"Error stopping server.");


			err = NetLib::Net::Dispose();
			Assert::IsTrue(err == NetLib::NetErrCode::neterr_noErr, L"Error disposing NetLib.");
			Logger::WriteMessage("NetLib is disposed.");

			Logger::WriteMessage("--- FINISH ---");
		}


		TEST_METHOD(ClientTryConnect)
		{
			Logger::WriteMessage("--- START ---");
			NetLib::NetErrCode err;

			err = NetLib::Net::Init();
			Assert::IsTrue(err == NetLib::NetErrCode::neterr_noErr, L"Error initializing NetLib.");
			Logger::WriteMessage("NetLib is initialized.");


			NetLib::Client client;
			Logger::WriteMessage("Client is created.");


			err = client.Connect("127.0.0.1", 1);
			Assert::IsTrue(err == NetLib::NetErrCode::neterr_connectionRefused, L"Connection should be refused but it succeeded.");


			err = NetLib::Net::Dispose();
			Assert::IsTrue(err == NetLib::NetErrCode::neterr_noErr, L"Error disposing NetLib.");
			Logger::WriteMessage("NetLib is disposed.");

			Logger::WriteMessage("--- FINISH ---");
		}
		

		TEST_METHOD(CheckIsListening)
		{
			Logger::WriteMessage("--- START ---");
			NetLib::NetErrCode err;

			err = NetLib::Net::Init();
			Assert::IsTrue(err == NetLib::NetErrCode::neterr_noErr, L"Error initializing NetLib.");
			Logger::WriteMessage("NetLib is initialized.");


			NetLib::Server server(nullptr, nullptr, nullptr);
			Assert::IsFalse(server.IsListening(), L"Server is listening after initialization.");
			Logger::WriteMessage("Server is initialized and is not listening.");


			err = server.Start(32167);
			Assert::IsTrue(err == NetLib::NetErrCode::neterr_noErr, L"Error starting server.");
			Assert::IsTrue(server.IsListening(), L"Server is not listening after start.");
			Logger::WriteMessage("Server is started and is listening.");


			err = server.Stop();
			Assert::IsTrue(err == NetLib::NetErrCode::neterr_noErr, L"Error stopping server.");

			// Wait some time to stop the server
			Sleep(100);

			Assert::IsFalse(server.IsListening(), L"Server is listening after stop.");
			Logger::WriteMessage("Server is stopped and is not listening.");


			err = NetLib::Net::Dispose();
			Assert::IsTrue(err == NetLib::NetErrCode::neterr_noErr, L"Error disposing NetLib.");
			Logger::WriteMessage("NetLib is disposed.");

			Logger::WriteMessage("--- FINISH ---");
		}


		TEST_METHOD(SendToServer)
		{
			Logger::WriteMessage("--- START ---");
			NetLib::NetErrCode err;

			err = NetLib::Net::Init();
			Assert::IsTrue(err == NetLib::NetErrCode::neterr_noErr, L"Error initializing NetLib.");
			Logger::WriteMessage("NetLib is initialized.");


			receivedSomething = false;
			NetLib::Server server(nullptr, nullptr, OnClientDataReceived);
			err = server.Start(32167);
			Assert::IsTrue(err == NetLib::NetErrCode::neterr_noErr, L"Error starting server.");
			Logger::WriteMessage("Server is started.");


			NetLib::Client client;
			err = client.Connect("127.0.0.1", 32167);
			Assert::IsTrue(err == NetLib::NetErrCode::neterr_noErr, L"Error connecting to server.");
			Logger::WriteMessage("Client connected.");


			std::string testMessage = "Hello, world!";
			err = client.Send(testMessage.c_str(), testMessage.size());
			Assert::IsTrue(err == NetLib::NetErrCode::neterr_noErr, L"Error sending test message to server.");
			Logger::WriteMessage("Test message sent.");


			err = client.Disconnect();
			Assert::IsTrue(err == NetLib::NetErrCode::neterr_noErr, L"Error disconnecting client from server.");
			Logger::WriteMessage("Client disconnected.");


			Sleep(100); // wait for data receive


			Assert::IsTrue(receivedSomething, L"Didn't receive test message.");
			Assert::IsTrue(receivedText.compare(testMessage) == 0, L"Received message doesn't match.");
			Logger::WriteMessage("Message received correct.");

			err = server.Stop();
			Assert::IsTrue(err == NetLib::NetErrCode::neterr_noErr, L"Error stopping server.");


			// Wait some time to stop the server
			Sleep(100);


			err = NetLib::Net::Dispose();
			Assert::IsTrue(err == NetLib::NetErrCode::neterr_noErr, L"Error disposing NetLib.");
			Logger::WriteMessage("NetLib is disposed.");

			Logger::WriteMessage("--- FINISH ---");
		}

		TEST_METHOD(SendToClient)
		{
			Logger::WriteMessage("--- START ---");
			NetLib::NetErrCode err;

			err = NetLib::Net::Init();
			Assert::IsTrue(err == NetLib::NetErrCode::neterr_noErr, L"Error initializing NetLib.");
			Logger::WriteMessage("NetLib is initialized.");


			receivedSomething = false;
			NetLib::Server server(OnClientAccepted, nullptr, OnClientDataReceived);
			err = server.Start(32167);
			Assert::IsTrue(err == NetLib::NetErrCode::neterr_noErr, L"Error starting server.");
			Logger::WriteMessage("Server is started.");


			NetLib::Client client;
			err = client.Connect("127.0.0.1", 32167);
			Assert::IsTrue(err == NetLib::NetErrCode::neterr_noErr, L"Error connecting to server.");
			Logger::WriteMessage("Client connected.");


			Sleep(100); // wait for connect


			std::string testMessage = "Hello, world!";
			err = server.SendToClient(clientId, testMessage.c_str(), testMessage.size());
			Assert::IsTrue(err == NetLib::NetErrCode::neterr_noErr, L"Error sending test message to client.");
			Logger::WriteMessage("Test message sent.");


			Sleep(100); // wait for data receive

			receivedText.resize(1024);
			int bytesReceived = 0;
			client.Receive(&receivedText[0], receivedText.size(), bytesReceived);
			receivedText.resize(bytesReceived);


			Assert::IsTrue(bytesReceived != 0, L"Didn't receive test message.");
			Assert::IsTrue(receivedText.compare(testMessage) == 0, L"Received message doesn't match.");
			Logger::WriteMessage("Message received correct.");


			err = client.Disconnect();
			Assert::IsTrue(err == NetLib::NetErrCode::neterr_noErr, L"Error disconnecting client from server.");
			Logger::WriteMessage("Client disconnected.");


			err = server.Stop();
			Assert::IsTrue(err == NetLib::NetErrCode::neterr_noErr, L"Error stopping server.");


			// Wait some time to stop the server
			Sleep(100);


			err = NetLib::Net::Dispose();
			Assert::IsTrue(err == NetLib::NetErrCode::neterr_noErr, L"Error disposing NetLib.");
			Logger::WriteMessage("NetLib is disposed.");

			Logger::WriteMessage("--- FINISH ---");
		}

	};
}