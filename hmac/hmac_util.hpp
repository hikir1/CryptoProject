#ifndef HMAC_UTIL_H_
#define HMAC_UTIL_H_

#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <sstream>
#include <random>
#include <gmpxx.h>
#include <unordered_map>
#include <unistd.h>
#include <bits/stdc++.h>
#include <algorithm>
#include <stdio.h>
#include <tgmath.h>
#include <cstdint>
#include <fstream>
#include <iomanip>

namespace hmac_util{
	unsigned long long int Bin_to_Dec(unsigned long long int given);
	std::string addBin(std::string first, std::string second);
	std::string xor_string(std::string first, std::string second);
	std::string and_string(std::string first, std::string second);
	std::string not_string(std::string reverse);
	std::string or_string(std::string first, std::string second);
	void conversionMap(std::unordered_map<std::string, char> *bth);
	std::string Hex_to_Bin(std::string given);
	std::string Bin_to_Hex(std::string given);
	std::string Dec_to_Bin(unsigned long long int value, bool flag=false);
	std::string convert_binary(std::string swapped);
	std::string convert_plaintext(std::string answer);
}
#endif