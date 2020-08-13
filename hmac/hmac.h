#ifndef HMAC_H_
#define HMAC_H_

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>

namespace hmac{
	constexpr int byte_length = 64;
	constexpr int output_length = 40;
	std::string create_HMAC(std::string message, const char* ce);
	void generate();
	extern std::string ipad;
	extern std::string opad;
}
#endif
