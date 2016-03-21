#include "stdafx.h"
#include "CppUnitTest.h"


#include "Server.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace NetLib_test
{		
	TEST_CLASS(NetLib_test)
	{
	public:
		
		TEST_METHOD(CheckIsListening)
		{
			NetLib::NetErrCode err;

			err = NetLib::Net::Init();
			Assert::IsTrue(err == NetLib::NetErrCode::neterr_noErr, L"Error initializing NetLib.");
			Logger::WriteMessage("NetLib is initialized.");


			NetLib::Server server;
			Assert::IsFalse(server.IsListening(), L"Server is listening after initialization.");
			Logger::WriteMessage("Server is initialized and is not listening.");


			err = server.Start(32167, nullptr);
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
		}

	};
}