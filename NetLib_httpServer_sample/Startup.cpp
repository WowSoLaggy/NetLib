#include "stdafx.h"


int main(int argc, char *argv[])
{
	std::string uriString = "http://user:pass@www.cplusplus.com:3599/reference/string/string/find_first_of/?par1=val1&par2=val2#my_anchor";

	NetLib::Uri uri(uriString);

	std::cout << uriString << "\n";
	std::cout << uri.ToStringWithCredentials() << "\n";
	std::cout << uri.ToString() << "\n";

	std::getchar();

	return 0;
}
