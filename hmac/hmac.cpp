#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>
#include "sha1.h"
#include "hmac.h"
#include "hmac_util.hpp"

std::string hmac::ipad = "";
std::string hmac::opad = "";

void hmac::generate(){
	ipad = "";
	opad = "";
	for(int x = 0; x < byte_length; x++){
		ipad = ipad + "00110110";
		opad = opad + "01011100";
	}
}

std::string hmac::create_HMAC(std::string message, const char* ce){
	//generates an HMAC from a message and key
	hmac::generate();
	std::string c(ce);
	std::string refurbished = c;
	std::string bin;
	std::string o_key = "";
	std::string i_key = "";
	if(c.length() > byte_length){
		refurbished = SHA1::run_sha(c);
		while(refurbished.length() < byte_length){
			refurbished = refurbished + SHA1::run_sha(refurbished + c);
		}
		refurbished = refurbished.substr(0, byte_length);
	}
	bin = hmac_util::convert_binary(refurbished);
	if(refurbished.length() < byte_length){
		while(bin.length() < byte_length*8){
			bin = bin + "0";	
		}
	}
	refurbished = bin;
	o_key = hmac_util::xor_string(refurbished, opad);
	i_key = hmac_util::xor_string(refurbished, ipad);
	i_key = i_key + hmac_util::convert_binary(message);
	i_key = hmac_util::Hex_to_Bin(SHA1::run_sha(i_key));
	o_key = o_key + i_key;
	return SHA1::run_sha(o_key);
}
