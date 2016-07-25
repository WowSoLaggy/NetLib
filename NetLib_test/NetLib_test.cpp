#include "stdafx.h"
#include "CppUnitTest.h"


#include "NetLib.h"
using namespace NetLib;
using namespace NetLib::NetErrCodes;


using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace NetLib_test
{
	volatile bool receivedSomething;
	std::string receivedText;
	void OnReceivedDataFromClient(CLIENTID pClientId, char *pData, int pDataLength)
	{
		receivedSomething = true;
		receivedText = std::string(pData, pDataLength);
	}
	volatile CLIENTID clientId;
	void OnClientAccepted(CLIENTID pClientId, std::string pClientAddress, int pClientPort)
	{
		clientId = pClientId;
	}
	void OnReceivedDataFromServer(char *pData, int pDataLength)
	{
		receivedSomething = true;
		receivedText = std::string(pData, pDataLength);
	}

	TEST_CLASS(NetLib_test)
	{
	public:

		TEST_METHOD(NetLibInitDispose)
		{
			Logger::WriteMessage("--- START ---");
			NetErrCode err;

			err = NetLib::Net::Init();
			Assert::IsTrue(err == neterr_noErr, L"Error initializing NetLib.");
			Logger::WriteMessage("NetLib is initialized.");

			err = NetLib::Net::Dispose();
			Assert::IsTrue(err == neterr_noErr, L"Error disposing NetLib.");
			Logger::WriteMessage("NetLib is disposed.");

			Logger::WriteMessage("--- FINISH ---");
		}


		TEST_METHOD(ServerCreateDelete)
		{
			Logger::WriteMessage("--- START ---");
			NetErrCode err;

			err = NetLib::Net::Init();
			Assert::IsTrue(err == neterr_noErr, L"Error initializing NetLib.");
			Logger::WriteMessage("NetLib is initialized.");


			NetLib::Server server(nullptr, nullptr, nullptr);
			Logger::WriteMessage("Server is initialized.");


			err = server.Stop();
			Assert::IsTrue(err == neterr_noErr, L"Error stopping server.");
			Logger::WriteMessage("Server stopped OK.");


			err = server.Start(32167);
			Assert::IsTrue(err == neterr_noErr, L"Error restarting server.");
			Logger::WriteMessage("Server restarted OK.");

			err = server.Stop();
			Assert::IsTrue(err == neterr_noErr, L"Error stopping server.");
			Logger::WriteMessage("Server stopped OK.");


			// Give time to stop
			Sleep(100);


			err = NetLib::Net::Dispose();
			Assert::IsTrue(err == neterr_noErr, L"Error disposing NetLib.");
			Logger::WriteMessage("NetLib is disposed.");

			Logger::WriteMessage("--- FINISH ---");
		}


		TEST_METHOD(ServerCreateTwice)
		{
			Logger::WriteMessage("--- START ---");
			NetErrCode err;

			err = NetLib::Net::Init();
			Assert::IsTrue(err == neterr_noErr, L"Error initializing NetLib.");
			Logger::WriteMessage("NetLib is initialized.");


			NetLib::Server server1(nullptr, nullptr, nullptr);
			Logger::WriteMessage("First server is initialized.");

			err = server1.Start(32167);
			Assert::IsTrue(err == neterr_noErr, L"Error starting server.");
			Logger::WriteMessage("First server started.");

			NetLib::Server server2(nullptr, nullptr, nullptr);
			Logger::WriteMessage("Second server is initialized.");

			err = server2.Start(32167);
			Assert::IsTrue(err == neterr_cantBindSocket, L"Second server started OK but should fail.");
			Logger::WriteMessage("Second server failed to start.");


			err = server1.Stop();
			Assert::IsTrue(err == neterr_noErr, L"Error stopping first server.");
			Logger::WriteMessage("First server stopped OK.");

			err = server2.Stop();
			Assert::IsTrue(err == neterr_noErr, L"Error stopping second server.");
			Logger::WriteMessage("Second server stopped OK.");


			// Give time to stop
			Sleep(100);


			err = NetLib::Net::Dispose();
			Assert::IsTrue(err == neterr_noErr, L"Error disposing NetLib.");
			Logger::WriteMessage("NetLib is disposed.");

			Logger::WriteMessage("--- FINISH ---");
		}


		TEST_METHOD(ClientTryConnect)
		{
			Logger::WriteMessage("--- START ---");
			NetErrCode err;

			err = NetLib::Net::Init();
			Assert::IsTrue(err == neterr_noErr, L"Error initializing NetLib.");
			Logger::WriteMessage("NetLib is initialized.");


			NetLib::Client client(OnReceivedDataFromServer);
			Logger::WriteMessage("Client is created.");


			err = client.Connect("127.0.0.1", 1);
			Assert::IsTrue(err == neterr_connectionRefused, L"Connection should be refused but it succeeded.");
			Logger::WriteMessage("Connection has been successfully refused.");


			err = NetLib::Net::Dispose();
			Assert::IsTrue(err == neterr_noErr, L"Error disposing NetLib.");
			Logger::WriteMessage("NetLib is disposed.");

			Logger::WriteMessage("--- FINISH ---");
		}
		

		TEST_METHOD(CheckIsListening)
		{
			Logger::WriteMessage("--- START ---");
			NetErrCode err;

			err = NetLib::Net::Init();
			Assert::IsTrue(err == neterr_noErr, L"Error initializing NetLib.");
			Logger::WriteMessage("NetLib is initialized.");


			NetLib::Server server(nullptr, nullptr, nullptr);
			Assert::IsFalse(server.IsListening(), L"Server is listening after initialization.");
			Logger::WriteMessage("Server is initialized and is not listening.");


			err = server.Start(32167);
			Assert::IsTrue(err == neterr_noErr, L"Error starting server.");
			Assert::IsTrue(server.IsListening(), L"Server is not listening after start.");
			Logger::WriteMessage("Server is started and is listening.");


			err = server.Stop();
			Assert::IsTrue(err == neterr_noErr, L"Error stopping server.");

			// Wait some time to stop the server
			Sleep(100);

			Assert::IsFalse(server.IsListening(), L"Server is listening after stop.");
			Logger::WriteMessage("Server is stopped and is not listening.");


			err = NetLib::Net::Dispose();
			Assert::IsTrue(err == neterr_noErr, L"Error disposing NetLib.");
			Logger::WriteMessage("NetLib is disposed.");

			Logger::WriteMessage("--- FINISH ---");
		}


		TEST_METHOD(SendToServer)
		{
			Logger::WriteMessage("--- START ---");
			NetErrCode err;

			err = NetLib::Net::Init();
			Assert::IsTrue(err == neterr_noErr, L"Error initializing NetLib.");
			Logger::WriteMessage("NetLib is initialized.");


			receivedSomething = false;
			NetLib::Server server(nullptr, nullptr, OnReceivedDataFromClient);
			err = server.Start(32167);
			Assert::IsTrue(err == neterr_noErr, L"Error starting server.");
			Logger::WriteMessage("Server is started.");


			NetLib::Client client(OnReceivedDataFromServer);
			err = client.Connect("127.0.0.1", 32167);
			Assert::IsTrue(err == neterr_noErr, L"Error connecting to server.");
			Logger::WriteMessage("Client connected.");


			std::string testMessage = "Hello, world!";
			err = client.SendToServer(testMessage.c_str(), (int)testMessage.size());
			Assert::IsTrue(err == neterr_noErr, L"Error sending test message to server.");
			Logger::WriteMessage("Test message sent.");


			err = client.Disconnect();
			Assert::IsTrue(err == neterr_noErr, L"Error disconnecting client from server.");
			Logger::WriteMessage("Client disconnected.");


			Sleep(100); // wait for data receive


			Assert::IsTrue(receivedSomething, L"Didn't receive test message.");
			Assert::IsTrue(receivedText.compare(testMessage) == 0, L"Received message doesn't match.");
			Logger::WriteMessage("Message received correct.");

			err = server.Stop();
			Assert::IsTrue(err == neterr_noErr, L"Error stopping server.");


			// Wait some time to stop the server
			Sleep(100);


			err = NetLib::Net::Dispose();
			Assert::IsTrue(err == neterr_noErr, L"Error disposing NetLib.");
			Logger::WriteMessage("NetLib is disposed.");

			Logger::WriteMessage("--- FINISH ---");
		}

		TEST_METHOD(SendToClient)
		{
			Logger::WriteMessage("--- START ---");
			NetErrCode err;

			err = NetLib::Net::Init();
			Assert::IsTrue(err == neterr_noErr, L"Error initializing NetLib.");
			Logger::WriteMessage("NetLib is initialized.");


			receivedSomething = false;
			NetLib::Server server(OnClientAccepted, nullptr, OnReceivedDataFromClient);
			err = server.Start(32167);
			Assert::IsTrue(err == neterr_noErr, L"Error starting server.");
			Logger::WriteMessage("Server is started.");


			NetLib::Client client(OnReceivedDataFromServer);
			err = client.Connect("127.0.0.1", 32167);
			Assert::IsTrue(err == neterr_noErr, L"Error connecting to server.");
			Logger::WriteMessage("Client connected.");


			Sleep(100); // wait for connect


			std::string testMessage = "Hello, world!";
			err = server.SendToClient(clientId, testMessage.c_str(), (int)testMessage.size());
			Assert::IsTrue(err == neterr_noErr, L"Error sending test message to client.");
			Logger::WriteMessage("Test message sent.");


			Sleep(100); // wait for data receive


			Assert::IsTrue(receivedSomething, L"Didn't receive test message.");
			Assert::IsTrue(receivedText.compare(testMessage) == 0, L"Received message doesn't match.");
			Logger::WriteMessage("Message received correct.");


			err = client.Disconnect();
			Assert::IsTrue(err == neterr_noErr, L"Error disconnecting client from server.");
			Logger::WriteMessage("Client disconnected.");


			err = server.Stop();
			Assert::IsTrue(err == neterr_noErr, L"Error stopping server.");


			// Wait some time to stop the server
			Sleep(100);


			err = NetLib::Net::Dispose();
			Assert::IsTrue(err == neterr_noErr, L"Error disposing NetLib.");
			Logger::WriteMessage("NetLib is disposed.");

			Logger::WriteMessage("--- FINISH ---");
		}

		TEST_METHOD(CheckClientExists)
		{
			Logger::WriteMessage("--- START ---");
			NetErrCode err;

			err = NetLib::Net::Init();
			Assert::IsTrue(err == neterr_noErr, L"Error initializing NetLib.");
			Logger::WriteMessage("NetLib is initialized.");


			receivedSomething = false;
			NetLib::Server server(OnClientAccepted, nullptr, nullptr);
			err = server.Start(32167);
			Assert::IsTrue(err == neterr_noErr, L"Error starting server.");
			Logger::WriteMessage("Server is started.");


			NetLib::Client client(OnReceivedDataFromServer);
			err = client.Connect("127.0.0.1", 32167);
			Assert::IsTrue(err == neterr_noErr, L"Error connecting to server.");
			Logger::WriteMessage("Client connected.");


			// Give some time to connect
			Sleep(100);


			Assert::IsTrue(server.CheckClientExists(clientId), L"Server says that connected client doesn't exist.");
			Assert::IsTrue(!server.CheckClientExists(12312377), L"Server says that random clientId exists.");
			Logger::WriteMessage("Server CheckClientExists function seems to work fine.");


			err = client.Disconnect();
			Assert::IsTrue(err == neterr_noErr, L"Error disconnecting client from server.");
			Logger::WriteMessage("Client disconnected.");


			err = server.Stop();
			Assert::IsTrue(err == neterr_noErr, L"Error stopping server.");


			// Wait some time to stop the server
			Sleep(100);


			err = NetLib::Net::Dispose();
			Assert::IsTrue(err == neterr_noErr, L"Error disposing NetLib.");
			Logger::WriteMessage("NetLib is disposed.");

			Logger::WriteMessage("--- FINISH ---");
		}

		TEST_METHOD(CheckUriParsing)
		{
			Logger::WriteMessage("--- START ---");
			std::string str;
			std::string res;
			Uri uri;


			str = "www.example.com";
			uri.Parse(str);
			res = uri.ToString();
			Assert::IsTrue(res.compare(str) == 0, L"URI parse error 1.");

			str = "www.example.com/";
			uri.Parse(str);
			res = uri.ToString();
			Assert::IsTrue(res.compare(str) == 0, L"URI parse error 2.");

			str = "www.example.com:3030";
			uri.Parse(str);
			res = uri.ToString();
			Assert::IsTrue(res.compare(str) == 0, L"URI parse error 3.");

			str = "www.example.com:3030/";
			uri.Parse(str);
			res = uri.ToString();
			Assert::IsTrue(res.compare(str) == 0, L"URI parse error 4.");

			str = "http://example.com";
			uri.Parse(str);
			res = uri.ToString();
			Assert::IsTrue(res.compare(str) == 0, L"URI parse error 5.");

			str = "http://example.com:3030";
			uri.Parse(str);
			res = uri.ToString();
			Assert::IsTrue(res.compare(str) == 0, L"URI parse error 6.");

			str = "http://example.com/";
			uri.Parse(str);
			res = uri.ToString();
			Assert::IsTrue(res.compare(str) == 0, L"URI parse error 7.");

			str = "http://example.com:3030/";
			uri.Parse(str);
			res = uri.ToString();
			Assert::IsTrue(res.compare(str) == 0, L"URI parse error 8.");

			str = "http://example.com/:3030";
			uri.Parse(str);
			res = uri.ToString();
			Assert::IsTrue(res.compare(str) == 0, L"URI parse error 8.1.");

			str = "http://hello@example.com";
			uri.Parse(str);
			res = uri.ToStringWithCredentials();
			Assert::IsTrue(res.compare(str) == 0, L"URI parse error 9.");

			str = "http://hello:@example.com";
			uri.Parse(str);
			res = uri.ToStringWithCredentials();
			Assert::IsTrue(res.compare(str) == 0, L"URI parse error 10.");

			str = "http://:hello@example.com";
			uri.Parse(str);
			res = uri.ToStringWithCredentials();
			Assert::IsTrue(res.compare(str) == 0, L"URI parse error 11.");

			str = "http://hello:wow@example.com";
			uri.Parse(str);
			res = uri.ToStringWithCredentials();
			Assert::IsTrue(res.compare(str) == 0, L"URI parse error 12.");

			str = "http://hello:wow@example.com:333";
			uri.Parse(str);
			res = uri.ToStringWithCredentials();
			Assert::IsTrue(res.compare(str) == 0, L"URI parse error 13.");

			str = "http://hello:wow@example.com:333/";
			uri.Parse(str);
			res = uri.ToStringWithCredentials();
			Assert::IsTrue(res.compare(str) == 0, L"URI parse error 14.");

			str = "http://:wow@example.com:333";
			uri.Parse(str);
			res = uri.ToStringWithCredentials();
			Assert::IsTrue(res.compare(str) == 0, L"URI parse error 15.");

			str = "http://:wow@example.com:333/";
			uri.Parse(str);
			res = uri.ToStringWithCredentials();
			Assert::IsTrue(res.compare(str) == 0, L"URI parse error 16.");

			str = "http://:wow@example.com:333/my_super/path";
			uri.Parse(str);
			res = uri.ToStringWithCredentials();
			Assert::IsTrue(res.compare(str) == 0, L"URI parse error 17.");

			str = "http://:wow@example.com:333/my_super/path/";
			uri.Parse(str);
			res = uri.ToStringWithCredentials();
			Assert::IsTrue(res.compare(str) == 0, L"URI parse error 18.");

			str = "http://:wow@example.com:333/my_super/path/?par1=val1&par2=val2";
			uri.Parse(str);
			res = uri.ToStringWithCredentials();
			Assert::IsTrue(res.compare(str) == 0, L"URI parse error 19.");

			str = "http://:wow@example.com:333/my_super/path/#my_anchor";
			uri.Parse(str);
			res = uri.ToStringWithCredentials();
			Assert::IsTrue(res.compare(str) == 0, L"URI parse error 20.");

			str = "http://:wow@example.com:333/my_super/path/#my_anchor?par1=val1&par2=val2";
			uri.Parse(str);
			res = uri.ToStringWithCredentials();
			Assert::IsTrue(res.compare(str) == 0, L"URI parse error 21.");


			Logger::WriteMessage("--- FINISH ---");
		}

	};
}
