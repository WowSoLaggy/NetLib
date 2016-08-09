// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Module:		Base64
// Author:		Anton Egorov, dohxehapo@gmail.com
// Description:	Module contains Base64 encoding and decoding algorithms.
//				Algorithms accept string as an input and return string as a
//				result. To work with unsigned chars in general additional
//				(yet very simple) wrappers are required.
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


#pragma once

#ifndef ENCODINGS_BASE64_H
#define ENCODINGS_BASE64_H


#include <string>
#include <vector>


namespace Base64
{

	// Base64 encoding table
	static std::string g_base64EncodeTable =
		"ABCDEFGH"
		"IJKLMNOP"
		"QRSTUVWX"
		"YZabcdef"
		"ghijklmn"
		"opqrstuv"
		"wxyz0123"
		"456789+/";

	// Magic const that represents the number of trimmed symbols in the g_base64DecodeTable
	static const int g_base64DecodeTableOffset = 43;

	// Base64 decoding table, generated as a reversed encoding table and trimmed
	static std::vector<int> g_base64DecodeTable =
	{
		62, 0, 0, 0, 63, 52, 53, 54,
		55, 56, 57, 58, 59, 60, 61, 0,
		0, 0, 0, 0, 0, 0, 0, 1,
		2, 3, 4, 5, 6, 7, 8, 9,
		10, 11, 12, 13, 14, 15, 16, 17,
		18, 19, 20, 21, 22, 23, 24, 25,
		0, 0, 0, 0, 0, 0, 26, 27,
		28, 29, 30, 31, 32, 33, 34, 35,
		36, 37, 38, 39, 40, 41, 42, 43,
		44, 45, 46, 47, 48, 49, 50, 51
	};


	// Encodes the given string with the Base64 encoder and returns the result
	// Params:
	// [in] const std::string & pInput - string to encode
	static std::string Base64_Encode(const std::string &pInput)
	{
		// We will process 3 bytes at a time. The remainder will be appended in the end
		int remainder = pInput.size() % 3;
		int length3 = pInput.size() / 3;

		// length3 * 4 because every 3 bytes are encoded by 4 base64 symbols. (length3 + 1) to reserve 4 chars for remainder encoding
		int resLength = (remainder > 0) ? (length3 + 1) * 4 : length3 * 4;
		std::string res;
		res.reserve(resLength);

		int i;
		for (i = 0; i < length3; ++i)
		{
			res.push_back(g_base64EncodeTable[(pInput[i * 3 + 0] >> 2)]);
			res.push_back(g_base64EncodeTable[((pInput[i * 3 + 0] & 0x03) << 4) | ((pInput[i * 3 + 1] & 0xF0) >> 4)]);
			res.push_back(g_base64EncodeTable[((pInput[i * 3 + 1] & 0x0F) << 2) | ((pInput[i * 3 + 2] & 0xC0) >> 6)]);
			res.push_back(g_base64EncodeTable[(pInput[i * 3 + 2] & 0x3F)]);
		}

		if (remainder == 1)
		{
			res.push_back(g_base64EncodeTable[(pInput[i * 3 + 0] >> 2)]);
			res.push_back(g_base64EncodeTable[((pInput[i * 3 + 0] & 0x03) << 4)]);
			res.push_back('=');
			res.push_back('=');
		}
		else if (remainder == 2)
		{
			res.push_back(g_base64EncodeTable[(pInput[i * 3 + 0] >> 2)]);
			res.push_back(g_base64EncodeTable[((pInput[i * 3 + 0] & 0x03) << 4) | ((pInput[i * 3 + 1] & 0xF0) >> 4)]);
			res.push_back(g_base64EncodeTable[((pInput[i * 3 + 1] & 0x0F) << 2)]);
			res.push_back('=');
		}

		return res;
	}


	// Decodes the given string with the Base64 decoder and returns the result
	// Params:
	// [in] const std::string & pInput - string to decode
	static std::string Base64_Decode(const std::string &pInput)
	{
		// We will process 4 bytes at a time. Input base64 string must be multiple of 4
		int length4 = pInput.size() / 4;

		// length4 * 3 because every 3 original bytes are encoded by 4 base64 symbols
		std::string res;
		res.reserve(length4 * 3);

		if (pInput[pInput.size() - 1] == '=')
		{
			// If we have at least one '=' in the end, then we should parse the last 4 symbols not in the main loop
			--length4;
		}

		int i;
		for (i = 0; i < length4; ++i)
		{
			res.push_back((
				g_base64DecodeTable[pInput[i * 4 + 0] - g_base64DecodeTableOffset] << 2) |
				(g_base64DecodeTable[pInput[i * 4 + 1] - g_base64DecodeTableOffset] >> 4));
			res.push_back((
				g_base64DecodeTable[pInput[i * 4 + 1] - g_base64DecodeTableOffset] << 4) |
				(g_base64DecodeTable[pInput[i * 4 + 2] - g_base64DecodeTableOffset] >> 2));
			res.push_back((
				g_base64DecodeTable[pInput[i * 4 + 2] - g_base64DecodeTableOffset] << 6) |
				(g_base64DecodeTable[pInput[i * 4 + 3] - g_base64DecodeTableOffset]));
		}

		if (pInput[pInput.size() - 2] == '=')
		{
			res.push_back((
				g_base64DecodeTable[pInput[i * 4 + 0] - g_base64DecodeTableOffset] << 2) |
				(g_base64DecodeTable[pInput[i * 4 + 1] - g_base64DecodeTableOffset] >> 4));
		}
		else if (pInput[pInput.size() - 1] == '=')
		{
			res.push_back((
				g_base64DecodeTable[pInput[i * 4 + 0] - g_base64DecodeTableOffset] << 2) |
				(g_base64DecodeTable[pInput[i * 4 + 1] - g_base64DecodeTableOffset] >> 4));
			res.push_back((
				g_base64DecodeTable[pInput[i * 4 + 1] - g_base64DecodeTableOffset] << 4) |
				(g_base64DecodeTable[pInput[i * 4 + 2] - g_base64DecodeTableOffset] >> 2));
		}

		return res;
	}

} // ns Base64


#endif // ENCODINGS_BASE64_H
