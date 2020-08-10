#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>
#include <bits/stdc++.h> 
#include "sha1.h"
#include "../util.cpp"

std::string SHA1::circle_shift(int shift, std::string shifted){
	int len = shifted.length();
	std::string start = "";
	std::string end = "";
	std::string finished = shifted;
	while(shift > len){
		shift = shift - shifted.length();
	}
	for(int x = 0; x < shift; x++){
		if(len > 1){
		  start = finished.substr(0, 1);
		  //takes the rest of the string in
		  end = finished.substr(1, len - 1);
		  finished = end + start;
		}
	}
	return finished;
}

uint32_t SHA1::circle_shift_uint(int shift, uint32_t shifted){
	return (shifted << shift) | (shifted >> (32 - shift));
}

std::string SHA1::padding(std::string message, int length){
	//gets the length of the message
	std::string final = "" + message;
	int pad_size = length % 512;
	if(pad_size >= 448){
		pad_size = 512 - pad_size;
		final = final + "1";
		pad_size--;
		for(int x = 0; x < pad_size + 448; x++){
			final = final + "0";
		}
	}else{
		pad_size = 448 - pad_size;
		final = final + "1";
		pad_size--;
		for(int x = 0; x < pad_size; x++){
			final = final + "0";
		}
	}
	std::string ending = Dec_to_Bin(length);
	while(ending.length() < 64){
		ending = "0" + ending;
	}
	return final + ending;
}

uint32_t SHA1::nonlinear_funct(int t, uint32_t B, uint32_t C, uint32_t D){
	if (0 <= t && t <= 19){
		return (B&C)|((~B)&D);
	}else if(20 <= t && t <= 39){
		return B^C^D;
	}else if(40 <= t && t <= 59){
		return (B&C)|(B&D)|(C&D);
	}else if(60 <= t && t <= 79){
		return B^C^D;
	}else{
		return B^(D&C);
	}
}

inline void SHA1::convert(std::string &strs, std::vector<uint32_t> &integers){
    for (size_t i = 0; i < num_blocks; i++){
         integers[i] = (strs[4*i+0] & 0xff)<<24
        		   | (strs[4*i+1] & 0xff)<<16
                   | (strs[4*i+2] & 0xff)<<8
                   | (strs[4*i+3] & 0xff);
    }
}

void SHA1::iterations(std::string chunk){
	std::vector<uint32_t> W(80);
	uint32_t A = holder[0];
	uint32_t B = holder[1];
	uint32_t C = holder[2];
	uint32_t D = holder[3];
	uint32_t E = holder[4];
	unsigned long long int temp;
	int count = 0;
	std::vector<uint32_t> K(4); 
	K[0] = 0x5A827999;
	K[1] = 0x6ED9EBA1;
	K[2] = 0x8F1BBCDC; 
	K[3] = 0xCA62C1D6;
	convert(chunk, W);
	for (int x = 16; x < 80; x++){
		W[x] = circle_shift_uint(1, (W[x-3] ^ W[x-8] ^ W[x-14] ^ W[x-16]));
	}
	for(int x = 0; x < 80; x++){
		if(x%20 == 0 && x != 0){
			count++;
		}
		temp = circle_shift_uint(5, A) + nonlinear_funct(x, B, C, D) + E + W[x] + K[count];
		E = D;
		D = C;
		C = circle_shift_uint(30, B);
		B = A;
		A = temp;
	}
	holder[0] = holder[0] + A;
	holder[1] = holder[1] + B;
	holder[2] = holder[2] + C;
	holder[3] = holder[3] + D;
	holder[4] = holder[4] + E;
}

std::string SHA1::hash_string(std::string full_string){
	int chucks = full_string.length()/512;
	std::string full = "";
	std::string chunk = "";
	for(int x = 0; x < chucks; x++){
		chunk = full_string.substr((x*512), 512);
		iterations(chunk);
	}
	uint32_t A = circle_shift_uint(128,holder[0]);
	uint32_t B = circle_shift_uint(96,holder[1]);
	uint32_t C = circle_shift_uint(64,holder[2]);
	uint32_t D = circle_shift_uint(32,holder[3]);
	uint32_t E = holder[4];
	full = full + Bin_to_Hex(Dec_to_Bin(A)) + Bin_to_Hex(Dec_to_Bin(B))+Bin_to_Hex(Dec_to_Bin(C))+Bin_to_Hex(Dec_to_Bin(D))+Bin_to_Hex(Dec_to_Bin(E));
	return full;
}

void SHA1::reset_sha1(){
	holder[0] = 0x08de7a01;
	holder[1] = 0xdf05e29c;
	holder[2] = 0x7ef1613b;
	holder[3] = 0x3e2999b2;
	holder[4] = 0xcdefa923;
}

std::string SHA1::run_sha(std::string convert){
	if(convert.length() > pow(2,64)){
		perror("Error: Length of string exceeds capabilities");
		return "Error";
	}
	std::string padded = "";
	std::string hashed = "";
	reset_sha1();
	padded = padding(convert, convert.length());
	hashed = hash_string(padded);
	return hashed;
}