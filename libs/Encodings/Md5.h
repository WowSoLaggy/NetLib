// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Module:		MD5
// Author:		Anton Egorov, dohxehapo@gmail.com
// Description:	Module contains MD5 encoding algorithm and static tables used
//				in calculations. Algorithm accepts vector of bytes as an input
//				and returns the hex-formatted string as a result.
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

#ifndef ENCODINGS_MD5_H
#define ENCODINGS_MD5_H


#include <string>
#include <vector>
#include <stdint.h>
#include <iomanip>
#include <sstream>


namespace Md5
{

	static std::vector<int32_t> g_shiftMatrix(
	{
		7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
		5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
		4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
		6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
	});

	static std::vector<uint32_t> g_sinMatrix(
	{
		0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
		0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
		0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
		0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
		0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
		0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
		0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
		0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
	});

	inline static uint32_t F(uint32_t X, uint32_t Y, uint32_t Z) { return (X & Y) | (~X & Z); }
	inline static uint32_t G(uint32_t X, uint32_t Y, uint32_t Z) { return (X & Z) | (~Z & Y); }
	inline static uint32_t H(uint32_t X, uint32_t Y, uint32_t Z) { return (X ^ Y ^ Z); }
	inline static uint32_t I(uint32_t X, uint32_t Y, uint32_t Z) { return (Y ^ (~Z | X)); }

	inline static uint32_t RotateLeft(uint32_t X, int N) { return (X << N) | (X >> (32 - N)); }

	inline static void Round1(uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t k, uint32_t s, uint32_t i)
	{
		a = b + (RotateLeft(a + F(b, c, d) + k + i, s));
	}

	inline static void Round2(uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t k, uint32_t s, uint32_t i)
	{
		a = b + (RotateLeft(a + G(b, c, d) + k + i, s));
	}

	inline static void Round3(uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t k, uint32_t s, uint32_t i)
	{
		a = b + (RotateLeft(a + H(b, c, d) + k + i, s));
	}

	inline static void Round4(uint32_t &a, uint32_t b, uint32_t c, uint32_t d, uint32_t k, uint32_t s, uint32_t i)
	{
		a = b + (RotateLeft(a + I(b, c, d) + k + i, s));
	}

	inline static std::vector<uint32_t> GetIntBlocks(const std::vector<uint8_t> &pInput)
	{
		int length4 = pInput.size() / 4;

		std::vector<uint32_t> res;
		res.resize(length4);
		for (int i = 0; i < length4; ++i)
		{
			res[i] =
				(pInput[i * 4 + 0]) | (pInput[i * 4 + 1] << 8) |
				(pInput[i * 4 + 2] << 16) | (pInput[i * 4 + 3] << 24);
		}

		return res;
	}


	static std::string Md5_Encode(const std::vector<uint8_t> &pInput)
	{
		std::vector<uint8_t> input(pInput);

		// Input data length alignment

		int length64 = input.size() / 64;
		int remainder = input.size() % 64;
		if (remainder < 56)
		{
			++length64;
			input.reserve(length64 * 64);
			input.push_back(0x80);
			if (remainder < 55)
				input.insert(input.end(), 56 - remainder - 1, 0x00);
		}
		else
		{
			length64 += 2;
			input.reserve(length64 * 64);
			input.push_back(0x80);
			input.insert(input.end(), 56 - remainder - 1 + 64, 0x00);
		}

		// Appending the input length

		uint64_t inputLenght = pInput.size() * 8;
		input.push_back((unsigned char)((inputLenght & 0x00000000000000FF)));
		input.push_back((unsigned char)((inputLenght & 0x000000000000FF00) >> 8));
		input.push_back((unsigned char)((inputLenght & 0x0000000000FF0000) >> 16));
		input.push_back((unsigned char)((inputLenght & 0x00000000FF000000) >> 24));
		input.push_back((unsigned char)((inputLenght & 0x000000FF00000000) >> 32));
		input.push_back((unsigned char)((inputLenght & 0x0000FF0000000000) >> 40));
		input.push_back((unsigned char)((inputLenght & 0x00FF000000000000) >> 48));
		input.push_back((unsigned char)((inputLenght & 0xFF00000000000000) >> 56));

		// Buffer initialization

		uint32_t sumA = 0x67452301; // Magic initialization consts
		uint32_t sumB = 0xEFCDAB89; //
		uint32_t sumC = 0x98BADCFE; //
		uint32_t sumD = 0x10325476; //

		uint32_t A, B, C, D;

		for (int i = 0; i < length64; ++i)
		{
			A = sumA;
			B = sumB;
			C = sumC;
			D = sumD;

			std::vector<uint8_t> temp(input.begin() + i * 64, input.begin() + (i + 1) * 64);
			std::vector<uint32_t> blocks = GetIntBlocks(temp);

			// Round 1

			Round1(A, B, C, D, blocks[0], g_shiftMatrix[0], g_sinMatrix[0]);
			Round1(D, A, B, C, blocks[1], g_shiftMatrix[1], g_sinMatrix[1]);
			Round1(C, D, A, B, blocks[2], g_shiftMatrix[2], g_sinMatrix[2]);
			Round1(B, C, D, A, blocks[3], g_shiftMatrix[3], g_sinMatrix[3]);
			Round1(A, B, C, D, blocks[4], g_shiftMatrix[4], g_sinMatrix[4]);
			Round1(D, A, B, C, blocks[5], g_shiftMatrix[5], g_sinMatrix[5]);
			Round1(C, D, A, B, blocks[6], g_shiftMatrix[6], g_sinMatrix[6]);
			Round1(B, C, D, A, blocks[7], g_shiftMatrix[7], g_sinMatrix[7]);

			Round1(A, B, C, D, blocks[8], g_shiftMatrix[8], g_sinMatrix[8]);
			Round1(D, A, B, C, blocks[9], g_shiftMatrix[9], g_sinMatrix[9]);
			Round1(C, D, A, B, blocks[10], g_shiftMatrix[10], g_sinMatrix[10]);
			Round1(B, C, D, A, blocks[11], g_shiftMatrix[11], g_sinMatrix[11]);
			Round1(A, B, C, D, blocks[12], g_shiftMatrix[12], g_sinMatrix[12]);
			Round1(D, A, B, C, blocks[13], g_shiftMatrix[13], g_sinMatrix[13]);
			Round1(C, D, A, B, blocks[14], g_shiftMatrix[14], g_sinMatrix[14]);
			Round1(B, C, D, A, blocks[15], g_shiftMatrix[15], g_sinMatrix[15]);

			// Round 2

			Round2(A, B, C, D, blocks[1], g_shiftMatrix[16], g_sinMatrix[16]);
			Round2(D, A, B, C, blocks[6], g_shiftMatrix[17], g_sinMatrix[17]);
			Round2(C, D, A, B, blocks[11], g_shiftMatrix[18], g_sinMatrix[18]);
			Round2(B, C, D, A, blocks[0], g_shiftMatrix[19], g_sinMatrix[19]);
			Round2(A, B, C, D, blocks[5], g_shiftMatrix[20], g_sinMatrix[20]);
			Round2(D, A, B, C, blocks[10], g_shiftMatrix[21], g_sinMatrix[21]);
			Round2(C, D, A, B, blocks[15], g_shiftMatrix[22], g_sinMatrix[22]);
			Round2(B, C, D, A, blocks[4], g_shiftMatrix[23], g_sinMatrix[23]);

			Round2(A, B, C, D, blocks[9], g_shiftMatrix[24], g_sinMatrix[24]);
			Round2(D, A, B, C, blocks[14], g_shiftMatrix[25], g_sinMatrix[25]);
			Round2(C, D, A, B, blocks[3], g_shiftMatrix[26], g_sinMatrix[26]);
			Round2(B, C, D, A, blocks[8], g_shiftMatrix[27], g_sinMatrix[27]);
			Round2(A, B, C, D, blocks[13], g_shiftMatrix[28], g_sinMatrix[28]);
			Round2(D, A, B, C, blocks[2], g_shiftMatrix[29], g_sinMatrix[29]);
			Round2(C, D, A, B, blocks[7], g_shiftMatrix[30], g_sinMatrix[30]);
			Round2(B, C, D, A, blocks[12], g_shiftMatrix[31], g_sinMatrix[31]);

			// Round 3

			Round3(A, B, C, D, blocks[5], g_shiftMatrix[32], g_sinMatrix[32]);
			Round3(D, A, B, C, blocks[8], g_shiftMatrix[33], g_sinMatrix[33]);
			Round3(C, D, A, B, blocks[11], g_shiftMatrix[34], g_sinMatrix[34]);
			Round3(B, C, D, A, blocks[14], g_shiftMatrix[35], g_sinMatrix[35]);
			Round3(A, B, C, D, blocks[1], g_shiftMatrix[36], g_sinMatrix[36]);
			Round3(D, A, B, C, blocks[4], g_shiftMatrix[37], g_sinMatrix[37]);
			Round3(C, D, A, B, blocks[7], g_shiftMatrix[38], g_sinMatrix[38]);
			Round3(B, C, D, A, blocks[10], g_shiftMatrix[39], g_sinMatrix[39]);

			Round3(A, B, C, D, blocks[13], g_shiftMatrix[40], g_sinMatrix[40]);
			Round3(D, A, B, C, blocks[0], g_shiftMatrix[41], g_sinMatrix[41]);
			Round3(C, D, A, B, blocks[3], g_shiftMatrix[42], g_sinMatrix[42]);
			Round3(B, C, D, A, blocks[6], g_shiftMatrix[43], g_sinMatrix[43]);
			Round3(A, B, C, D, blocks[9], g_shiftMatrix[44], g_sinMatrix[44]);
			Round3(D, A, B, C, blocks[12], g_shiftMatrix[45], g_sinMatrix[45]);
			Round3(C, D, A, B, blocks[15], g_shiftMatrix[46], g_sinMatrix[46]);
			Round3(B, C, D, A, blocks[2], g_shiftMatrix[47], g_sinMatrix[47]);

			// Round 4

			Round4(A, B, C, D, blocks[0], g_shiftMatrix[48], g_sinMatrix[48]);
			Round4(D, A, B, C, blocks[7], g_shiftMatrix[49], g_sinMatrix[49]);
			Round4(C, D, A, B, blocks[14], g_shiftMatrix[50], g_sinMatrix[50]);
			Round4(B, C, D, A, blocks[5], g_shiftMatrix[51], g_sinMatrix[51]);
			Round4(A, B, C, D, blocks[12], g_shiftMatrix[52], g_sinMatrix[52]);
			Round4(D, A, B, C, blocks[3], g_shiftMatrix[53], g_sinMatrix[53]);
			Round4(C, D, A, B, blocks[10], g_shiftMatrix[54], g_sinMatrix[54]);
			Round4(B, C, D, A, blocks[1], g_shiftMatrix[55], g_sinMatrix[55]);

			Round4(A, B, C, D, blocks[8], g_shiftMatrix[56], g_sinMatrix[56]);
			Round4(D, A, B, C, blocks[15], g_shiftMatrix[57], g_sinMatrix[57]);
			Round4(C, D, A, B, blocks[6], g_shiftMatrix[58], g_sinMatrix[58]);
			Round4(B, C, D, A, blocks[13], g_shiftMatrix[59], g_sinMatrix[59]);
			Round4(A, B, C, D, blocks[4], g_shiftMatrix[60], g_sinMatrix[60]);
			Round4(D, A, B, C, blocks[11], g_shiftMatrix[61], g_sinMatrix[61]);
			Round4(C, D, A, B, blocks[2], g_shiftMatrix[62], g_sinMatrix[62]);
			Round4(B, C, D, A, blocks[9], g_shiftMatrix[63], g_sinMatrix[63]);

			sumA += A;
			sumB += B;
			sumC += C;
			sumD += D;
		}

		// Encode the result digest to a hex string.
		// So much std::setw because it is not sticky
		std::stringstream sstream;
		sstream << std::setfill('0') << std::setw(2) << std::hex << 
			((sumA & 0x000000FF)) << std::setw(2) <<
			((sumA & 0x0000FF00) >> 8) << std::setw(2) <<
			((sumA & 0x00FF0000) >> 16) << std::setw(2) <<
			((sumA & 0xFF000000) >> 24) << std::setw(2) <<
			((sumB & 0x000000FF)) << std::setw(2) <<
			((sumB & 0x0000FF00) >> 8) << std::setw(2) <<
			((sumB & 0x00FF0000) >> 16) << std::setw(2) <<
			((sumB & 0xFF000000) >> 24) << std::setw(2) <<
			((sumC & 0x000000FF)) << std::setw(2) <<
			((sumC & 0x0000FF00) >> 8) << std::setw(2) <<
			((sumC & 0x00FF0000) >> 16) << std::setw(2) <<
			((sumC & 0xFF000000) >> 24) << std::setw(2) <<
			((sumD & 0x000000FF)) << std::setw(2) <<
			((sumD & 0x0000FF00) >> 8) << std::setw(2) <<
			((sumD & 0x00FF0000) >> 16) << std::setw(2) <<
			((sumD & 0xFF000000) >> 24);
		std::string res(sstream.str());

		return res;
	}

} // ns Md5


#endif // ENCODINGS_MD5_H
