#include <iostream>
#include <stdio.h>
#include <std::string>
#include <vector>
#include <algorithm>
#include <bits/stdc++.h> 
#include "sha1.h"

std::string padding(std::string message, int length){
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

std::string nonlinear_funct(int t, std::string B, std::string C, std::string D){
	B = Hex_to_Bin(B);
	C = Hex_to_Bin(C);
	D = Hex_to_Bin(D);
	if (0 <= t <= 19){
		return or_string(and_string(B,C),and_string(not_string(B),D));
	}else if(20 <= t <= 39){
		return xor_string(xor_string(B,C),D);
	}else if(40 <= t <= 59){
		return or_string(and_string(B,C), or_string(and_string(B,D),and_string(C,D)));
	}else if(60 <= t <= 79){
		return xor_string(xor_string(B,C),D);
	}
}

void iterations(std::string chunk, SHA1 *sha){
	transformations = 0;
	std::string W[80];
	std::string A = sha->holder[0];
	std::string B = sha->holder[1];
	std::string C = sha->holder[2];
	std::string D = sha->holder[3];
	std::string E = sha->holder[4];
	std::string temp = "";
	int count = 0;
	const std::string K[] = {"5A827999", "6ED9EBA1", "8F1BBCDC", "CA62C1D6"};
	for (int x = 0; x < 16; x++){
		W[x] = chunk.substr(x*4,4);
	}
	for (int x = 16; x < 80; x++){
		W[x] = circle_shift(1, xor_string(xor_string(xor_string(W[x-3], W[x-8]), W[x-14]), W[x-16]));
	}
	for(int x = 0; x < 80; x++){
		if(x%20 == 0){
			count++;
		}
		temp = addBin(addBin(circle_shift(5, Hex_to_Bin(A)), nonlinear_funct(x, B, C, D)), Hex_to_Bin(E));
		temp = Bin_to_Hex(addBin(addBin(temp,W[x]), K[count]));
		E = D;
		D = C;
		C = circle_shift(30, B);
		B = A;
		A = temp;
	}
	sha->holder[0] = Bin_to_Hex(addBin(Hex_to_Bin(sha->holder[0]), Hex_to_Bin(A)));
	sha->holder[1] = Bin_to_Hex(addBin(Hex_to_Bin(sha->holder[1]), Hex_to_Bin(B)));
	sha->holder[2] = Bin_to_Hex(addBin(Hex_to_Bin(sha->holder[2]), Hex_to_Bin(C)));
	sha->holder[3] = Bin_to_Hex(addBin(Hex_to_Bin(sha->holder[3]), Hex_to_Bin(D)));
	sha->holder[4] = Bin_to_Hex(addBin(Hex_to_Bin(sha->holder[4]), Hex_to_Bin(E)));
}

std::string hash_string(string full_string, SHA1 *sha){
	int chucks = full_string.length()/512;
	string full = "";
	string chunk = "";
	for(int x = 0; x < chucks; x++){
		chunk = full_string.substr((x*512), 512);
		iterations(chunk, &sha);
	}

	return full;
}

void reset_sha1(SHA1 *sha){
	sha->holder[0] = "08de7a01";
	sha->holder[1] = "df05e29c";
	sha->holder[2] = "7ef1613b";
	sha->holder[3] = "3e2999b2";
	sha->holder[4] = "cdefa923";
	transformations = 0;
}

std::string run_sha(std::string convert){
	SHA1 sha;
	std::string padded = "";
	std::string hashed = "";
	reset_sha1(&sha);
	padded = padding(convert, convert.length());
	hashed = hash_string(padded, &sha);
	std::string final[150];
	strcpy(final, hashed);
	return final;
}

