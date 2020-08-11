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
	extern const int byte_length;
	extern const int output_length;
	extern std::string ipad;
	extern std::string opad;
}
#endif