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
	static const int byte_length = 64;
	static const int output_length = 20;
	std::string ipad;
	std::string opad;
}
#endif