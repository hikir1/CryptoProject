#ifndef SHA1_H_
#define SHA1_H_

#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>

class SHA1{
	public:
		std::string run_sha1(std::string convert);
	private:
		static const int num_h = 5;
		static const int num_blocks = 16;
		int transformation = 0;
		std::string holder[5];
		void reset_sha1(SHA1 *sha);
		std::string padding(std::string message, int message_length);
		std::string nonlinear_funct(int t, std::string B, std::string C, std::string D);
		void iterations(std::string chunk, SHA1 *sha);
		std::string hash_string(string full_string, SHA1 *sha);
}



#endif

