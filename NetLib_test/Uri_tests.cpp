#include "stdafx.h"
#include "CppUnitTest.h"


#include "NetLib.h"
using namespace NetLib;
using namespace NetLib::NetErrCodes;


using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace NetLib_test
{

	TEST_CLASS(Uri_test)
	{
	public:

		TEST_METHOD(CheckUriParsing)
		{
			Logger::WriteMessage("--- START ---");
			std::string str;
			std::string res;
			Uri uri;


			Logger::WriteMessage("Checking URI parsing.");


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

			str = "/my_super/path";
			uri.Parse(str);
			res = uri.ToStringWithCredentials();
			Assert::IsTrue(res.compare(str) == 0, L"URI parse error 22.");

			str = "/my_super/path/";
			uri.Parse(str);
			res = uri.ToStringWithCredentials();
			Assert::IsTrue(res.compare(str) == 0, L"URI parse error 23.");

			str = "/my_super/path/#my_anchor?par1=val1&par2=val2";
			uri.Parse(str);
			res = uri.ToStringWithCredentials();
			Assert::IsTrue(res.compare(str) == 0, L"URI parse error 24.");

			str = "/";
			uri.Parse(str);
			res = uri.ToStringWithCredentials();
			Assert::IsTrue(res.compare(str) == 0, L"URI parse error 25.");


			Logger::WriteMessage("URI parsing OK.");


			Logger::WriteMessage("--- FINISH ---");
		}

	};
}
