#include "stdafx.h"
#include "CppUnitTest.h"


#include "NetLib.h"
using namespace NetLib;
using namespace NetLib::NetErrCodes;


using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace NetLib_test
{

	TEST_CLASS(Utils_test)
	{
	public:

		TEST_METHOD(CheckIsSameDir)
		{
			std::string dir1;
			std::string dir2;

			dir1 = "c:\\";
			dir2 = "c:\\";
			Assert::IsTrue(IsSameDir(dir1, dir2), L"IsSameDir works bad 1.");

			dir1 = "c:\\Windows\\";
			dir2 = "c:\\Windows\\";
			Assert::IsTrue(IsSameDir(dir1, dir2), L"IsSameDir works bad 2.");

			dir1 = "c:\\Windows\\";
			dir2 = "c:\\Windows";
			Assert::IsTrue(IsSameDir(dir1, dir2), L"IsSameDir works bad 3.");

			dir1 = "C:\\WINDOWS\\";
			dir2 = "c:\\windows\\";
			Assert::IsTrue(IsSameDir(dir1, dir2), L"IsSameDir works bad 4.");

			dir1 = "C:\\WINDOWS\\";
			dir2 = "c:/windows/";
			Assert::IsTrue(IsSameDir(dir1, dir2), L"IsSameDir works bad 5.");

			dir1 = "C:////WINDOWS////";
			dir2 = "c:\\windows\\\\";
			Assert::IsTrue(IsSameDir(dir1, dir2), L"IsSameDir works bad 6.");

			dir1 = "c:/";
			dir2 = "c://Windows";
			Assert::IsFalse(IsSameDir(dir1, dir2), L"IsSameDir works bad 7.");

			dir1 = "c:";
			dir2 = "c://";
			Assert::IsFalse(IsSameDir(dir1, dir2), L"IsSameDir works bad 8.");

			dir1 = "c";
			dir2 = "c://";
			Assert::IsFalse(IsSameDir(dir1, dir2), L"IsSameDir works bad 9");

			Logger::WriteMessage("IsSameDir works fine.");
		}

		TEST_METHOD(CheckIsSubDir)
		{
			std::string subDir;
			std::string parDir;

			subDir = "c:\\Windows";
			parDir = "c:\\";
			Assert::IsTrue(IsSubDir(subDir, parDir), L"IsSubDir works bad 1.");

			subDir = "c:\\Windows\\";
			parDir = "c:\\Windows\\";
			Assert::IsFalse(IsSubDir(subDir, parDir), L"IsSubDir works bad 2.");

			subDir = "C:\\";
			parDir = "c:\\windows\\";
			Assert::IsFalse(IsSubDir(subDir, parDir), L"IsSubDir works bad 3.");

			subDir = "C:\\WINDOWS\\";
			parDir = "c:/";
			Assert::IsTrue(IsSubDir(subDir, parDir), L"IsSubDir works bad 4.");

			subDir = "C:////WINDOWS////SYSTEM32";
			parDir = "c:\\";
			Assert::IsTrue(IsSubDir(subDir, parDir), L"IsSubDir works bad 5.");

			subDir = "C:////WINDOWS////SYSTEM32";
			parDir = "c:\\windows\\\\";
			Assert::IsTrue(IsSubDir(subDir, parDir), L"IsSubDir works bad 6.");

			subDir = "..";
			parDir = "../..";
			Assert::IsTrue(IsSubDir(subDir, parDir), L"IsSubDir works bad 7.");

			Logger::WriteMessage("IsSubDir works fine.");
		}

	};
}
