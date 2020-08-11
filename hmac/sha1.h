#ifndef SHA1_H_
#define SHA1_H_

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>

namespace SHA1{
	std::string run_sha(std::string convert);
	extern static const int num_h = 5;
	extern static const int num_blocks = 16;
	extern int transformation = 0;
	std::vector<uint32_t> holder = std::vector<uint32_t>(5);
	void reset_sha1();
	std::string padding(std::string message, int message_length);
	uint32_t nonlinear_funct(int t, uint32_t B, uint32_t C, uint32_t D);
	void iterations(std::string chunk);
	std::string hash_string(std::string full_string);
	std::string circle_shift(int shift, std::string shifted);
	uint32_t circle_shift_uint(int shift, uint32_t shifted);
	inline void convert(std::string &strs, std::vector<uint32_t> &integers);
}
#endif
