#pragma once

#ifndef VERSIONRETRIEVER_H
#define VERSIONRETRIEVER_H


#pragma comment(lib, "version.lib")


namespace Log
{

	// Returns version of the product from the file with the given name
	// Params:
	// [in]  std::string pFilePath				- file to acquire version from
	// [out] std::string & strProductVersion	- string that contains version if any
	// Returns:
	// true		- version successfully acquired
	// false	- failed to get version
	bool GetProductVersion(std::string pFilePath, std::string &strProductVersion);

} // Log


#endif // VERSIONRETRIEVER_H
