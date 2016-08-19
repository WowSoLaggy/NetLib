#pragma once

#ifndef UTILS_H
#define UTILS_H


#define NOMINMAX
#include <Windows.h>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <fstream>
#include <filesystem>


namespace NetLib
{
	using std::tr2::sys::path;


	// Trims the given string from the start (removes all spaces)
	// Params:
	// [out] std::string & pString	- string to trim
	static inline void TrimStringLeadRef(std::string &pString)
	{
		pString.erase(pString.begin(), std::find_if(pString.begin(), pString.end(),
			std::not1(std::ptr_fun<int, int>(std::isspace))));
	}

	// Trims the given string from the end (removes all spaces)
	// Params:
	// [out] std::string & pString	- string to trim
	static inline void TrimStringTrailRef(std::string &pString)
	{
		pString.erase(std::find_if(pString.rbegin(), pString.rend(),
			std::not1(std::ptr_fun<int, int>(std::isspace))).base(), pString.end());
	}

	// Trims the given string (removes all spaces)
	// Params:
	// [out] std::string & pString	- string to trim
	static inline void TrimStringRef(std::string &pString)
	{
		TrimStringLeadRef(pString);
		TrimStringTrailRef(pString);
	}

	// Trims the given string from the start (removes all spaces)
	// Params:
	// [out] std::string pString	- string to trim
	static inline std::string TrimStringLead(std::string pString)
	{
		TrimStringLeadRef(pString);
		return pString;
	}

	// Trims the given string from the end (removes all spaces)
	// Params:
	// [out] std::string pString	- string to trim
	static inline std::string TrimStringTrail(std::string pString)
	{
		TrimStringTrailRef(pString);
		return pString;
	}

	// Trims the given string (removes all spaces)
	// Params:
	// [out] std::string pString	- string to trim
	static inline std::string TrimString(std::string pString)
	{
		TrimStringRef(pString);
		return pString;
	}


	// Splits the given string to tokens with a given delimiter
	// Params:
	// [in]  const std::string & pString		- string to split
	// [in]  char pDelimiter					- delimiter used to split the given string
	// [out] std::vector<std::string> & pTokens	- vector of splitted tokens
	// Returns:
	// std::vector<std::string>					- vector of splitted tokens (reference to pTokens)
	static std::vector<std::string> & SplitString(const std::string &pString, char pDelimiter, std::vector<std::string> &pTokens, bool pTrimTokens = false)
	{
		std::stringstream ss(pString);
		std::string item;
		if (pTrimTokens)
		{
			while (std::getline(ss, item, pDelimiter))
				pTokens.push_back(TrimString(item));
		}
		else
		{
			while (std::getline(ss, item, pDelimiter))
				pTokens.push_back(item);
		}

		return pTokens;
	}


	// Splits the given string to tokens with a given delimiter
	// Params:
	// [in] const std::string & pString	- string to split
	// [in] char pDelimiter				- delimiter used to split the given string
	// Returns:
	// std::vector<std::string>				- vector of splitted tokens
	static std::vector<std::string> SplitString(const std::string &pString, char pDelimiter, bool pTrimTokens = false)
	{
		std::vector<std::string> tokens;
		return SplitString(pString, pDelimiter, tokens, pTrimTokens);
	}


	// Converts the given string to the boolean value
	// Params:
	// [in] const std::string & pString	- string to convert
	static bool StringToBool(const std::string &pString)
	{
		std::string str = pString;
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		return (str.compare("true") == 0) || (str.compare("1") == 0);
	}


	// Reads whole given file to a string
	// Params:
	// [in] const std::string & pFileName	- name of the file to read from
	static std::string ReadFileToString(const std::string &pFileName)
	{
		std::ifstream fileStream(pFileName);
		return std::string(std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>());
	}


	// Returns current date in the Http format
	static std::string GetHttpDate()
	{
		std::vector<char> str(100, ' ');
		time_t now = time(0);
		struct tm tm;
		gmtime_s(&tm, &now);
		strftime(str.data(), str.size(), "%a, %d %b %Y %H:%M:%S %Z", &tm);
		return std::string(str.begin(), str.begin() + 29);
	}


	// Checks whether the file with the given name exists
	// Params:
	// [in] const std::string & pFileName	- name of the file to check for existance
	static bool CheckFileExists(const std::string &pFileName)
	{
		struct stat buffer;
		return (stat(pFileName.c_str(), &buffer) == 0);
	}


	// Check whether the input string is a number (can be parsed to)
	// Params:
	// [in] const std::string & pString - input string to be checked
	static bool IsNumber(const std::string &pString)
	{
		return (
			(!pString.empty()) &&
			(std::find_if(pString.begin(), pString.end(),
				[](char c) { return !std::isdigit(c); }) == pString.end())
			);
	}


	// Checks whether the two given paths point to the same directory.
	// Params:
	// [in] std::string pDir1	- path to the directory that supposed to be the sub-directory
	// [in] std::string pDir2	- path to the directory that supposed to be the parent directory
	static bool IsSameDir(std::string pDir1, std::string pDir2)
	{
		// This implementation is not as good as it should be,
		// but I don't want to do it anymore

		if ((!is_directory(path(pDir1))) || (!is_directory(path(pDir2))))
			return false;

		std::transform(pDir1.begin(), pDir1.end(), pDir1.begin(), ::tolower);
		std::transform(pDir2.begin(), pDir2.end(), pDir2.begin(), ::tolower);

		char absolute[MAX_PATH];
		_fullpath(absolute, pDir1.c_str(), MAX_PATH);
		pDir1 = absolute;
		_fullpath(absolute, pDir2.c_str(), MAX_PATH);
		pDir2 = absolute;

		while ((pDir1[pDir1.size() - 1] == '\\') || (pDir1[pDir1.size() - 1] == '/'))
			pDir1.erase(pDir1.end() - 1);
		while ((pDir2[pDir2.size() - 1] == '\\') || (pDir2[pDir2.size() - 1] == '/'))
			pDir2.erase(pDir2.end() - 1);

		return (pDir1.compare(pDir2) == 0);
	}


	// Checks whether the two given paths point to the same directory.
	// Params:
	// [in] path pDir1	- path to the directory that supposed to be the sub-directory
	// [in] path pDir2	- path to the directory that supposed to be the parent directory
	static bool IsSameDir(path pDir1, path pDir2)
	{
		return IsSameDir(pDir1.string(), pDir2.string());
	}

	
	// Checks whether the given pSubDir is a sub-directory for a pParentDir
	// Params:
	// [in] std::string pSubDir		- path to the directory that supposed to be the sub-directory
	// [in] std::string pParentDir	- path to the directory that supposed to be the parent directory
	static bool IsSubDir(std::string pSubDir, std::string pParentDir)
	{
		// This implementation is not as good as it should be,
		// but I don't want to do it anymore

		pSubDir = pSubDir.append("/");
		pParentDir = pParentDir.append("/");

		std::transform(pSubDir.begin(), pSubDir.end(), pSubDir.begin(), ::tolower);
		std::transform(pParentDir.begin(), pParentDir.end(), pParentDir.begin(), ::tolower);

		char absolute[MAX_PATH];
		_fullpath(absolute, pSubDir.c_str(), MAX_PATH);
		pSubDir = absolute;
		_fullpath(absolute, pParentDir.c_str(), MAX_PATH);
		pParentDir = absolute;

		while ((pParentDir[pParentDir.size() - 1] == '\\') || (pParentDir[pParentDir.size() - 1] == '/'))
			pParentDir.erase(pParentDir.end() - 1);
		while ((pSubDir[pSubDir.size() - 1] == '\\') || (pSubDir[pSubDir.size() - 1] == '/'))
			pSubDir.erase(pSubDir.end() - 1);

		path dirParent(pParentDir);
		path dirSub(pSubDir);

		if ((!is_directory(path(dirParent))) || (!is_directory(path(dirSub))))
			return false;

		while (dirSub.has_parent_path())
		{
			dirSub = dirSub.parent_path();
			if (dirSub.compare(dirParent) == 0)
				return true;
		}

		return false;
	}


	// Checks whether the given pSubDir is a sub-directory for a pParentDir
	// Params:
	// [in] path pSubDir		- path to the directory that supposed to be the sub-directory
	// [in] path pParentDir	- path to the directory that supposed to be the parent directory
	static bool IsSubDir(path pSubDir, path pParentDir)
	{
		return IsSubDir(pSubDir.string(), pParentDir.string());
	}

}; // ns NetLib


#endif // UTILS_H
