#pragma once

#ifndef UTILS_H
#define UTILS_H


#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <fstream>


namespace NetLib
{

	// Splits the given string to tokens with a given delimiter
	// Params:
	// [in]  const std::string & pString		- string to split
	// [in]  char pDelimiter					- delimiter used to split the given string
	// [out] std::vector<std::string> & pTokens	- vector of splitted tokens
	// Returns:
	// std::vector<std::string>					- vector of splitted tokens (reference to pTokens)
	static std::vector<std::string> & SplitString(const std::string &pString, char pDelimiter, std::vector<std::string> &pTokens)
	{
		std::stringstream ss(pString);
		std::string item;
		while (std::getline(ss, item, pDelimiter))
			pTokens.push_back(item);

		return pTokens;
	}


	// Splits the given string to tokens with a given delimiter
	// Params:
	// [in] const std::string & pString	- string to split
	// [in] char pDelimiter				- delimiter used to split the given string
	// Returns:
	// std::vector<std::string>				- vector of splitted tokens
	static std::vector<std::string> SplitString(const std::string &pString, char pDelimiter)
	{
		std::vector<std::string> tokens;
		return SplitString(pString, pDelimiter, tokens);
	}


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


	static bool StringToBool(std::string pString)
	{
		std::transform(pString.begin(), pString.end(), pString.begin(), ::tolower);
		return (pString.compare("true") == 0) || (pString.compare("1") == 0);
	}


	// Reads whole given file to a string
	// Params:
	// [in] const std::string & pFileName	- name of the file to read from
	static std::string ReadFileToString(const std::string &pFileName)
	{
		std::ifstream fileStream(pFileName);
		return std::string(std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>());
	}

}; // ns NetLib


#endif // UTILS_H
