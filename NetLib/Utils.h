#pragma once

#ifndef UTILS_H
#define UTILS_H


#include <string>
#include <sstream>
#include <vector>


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
	// [in]  const std::string & pString	- string to split
	// [in]  char pDelimiter				- delimiter used to split the given string
	// Returns:
	// std::vector<std::string>				- vector of splitted tokens
	static std::vector<std::string> SplitString(const std::string &pString, char pDelimiter)
	{
		std::vector<std::string> tokens;
		return SplitString(pString, pDelimiter, tokens);
	}

}; // ns NetLib


#endif // UTILS_H
