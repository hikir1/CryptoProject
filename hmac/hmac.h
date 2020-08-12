#ifndef HMAC_H_
#define HMAC_H_

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>

namespace hmac{
	constexpr int byte_length = 64;
	constexpr int output_length = 20;
	std::string create_HMAC(std::string message, std::string c);
	void generate();
	extern std::string ipad;
	extern std::string opad;
}
#endif
