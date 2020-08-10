#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>
#include "sha1.h"
#include "hmac.h"


void hmac::generate(){
	ipad = "";
	opad = "";
	for(int x = 0; x < byte_length; x++){
		ipad = ipad + "00110110";
		opad = opad + "01011100";
	}
}

std::string hmac::create_HMAC(std::string message, std::string c){
	//generates an HMAC from a message and key
	hmac::generate();
	std::string refurbished = c;
	std::string bin;
	std::string o_key = "";
	std::string i_key = "";
	if(c.length() > byte_length){
		refurbished = sha1::run_sha(c);
		while(refurbished.length() < byte_length){
			refurbished = refurbished + sha1::run_sha(refurbished + c);
		}
		refurbished = refurbished.substr(0, byte_length);
	}
	bin = convert_binary(refurbished);
	if(refurbished.length() < byte_length){
		while(bin.length() < byte_length*8){
			bin = bin + "0";	
		}
	}
	refurbished = bin;
	o_key = xor_string(refurbished, opad);
	i_key = xor_string(refurbished, ipad);
	i_key = i_key + convert_binary(message);
	i_key = Hex_to_Bin(sha1::run_sha(i_key));
	o_key = o_key + i_key;
	return sha1::run_sha(o_key);
}