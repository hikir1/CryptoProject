#ifndef HMAC_H_
#define HMAC_H_

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>

namespace hmac{
	std::string create_HMAC(std::string message, std::string c);
	void generate();
	extern static const int byte_length = 64;
	extern static const int output_length = 20;
	extern std::string ipad;
	extern std::string opad;
}
#endif