#ifndef SHA1_H_
#define SHA1_H_

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>

namespace SHA1{
	class SHA1{
		public:
			std::string run_sha1(std::string convert);
		private:
			static const int num_h = 5;
			static const int num_blocks = 16;
			int transformation = 0;
			std::vector<uint32_t> holder(5);
			void reset_sha1();
			std::string padding(std::string message, int message_length);
			std::string nonlinear_funct(int t, uint32_t B, uint32_t C, uint32_t D);
			void iterations(std::string chunk);
			std::string hash_string(string full_string);
			std::string circle_shift(int shift, std::string shifted);
			uint32_t circle_shift_uint(int shift, uint32_t shifted);
			inline static void convert(std::string &strs, vector<uint32_t> &integers);
	}
}
#endif
