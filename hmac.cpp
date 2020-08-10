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
	for(int x = 0; )
}

std::string hmac::create_HMAC(std::string message, std::string c){
	generate();
	std::string refurbished = c;
	if(c.length() > byte_length){
		refurbished = run_sha(c);
		while(refurbished.length() < byte_length){
			refurbished = refurbished + run_sha(refurbished + c);
		}
		refurbished = refurbished.substr(0, byte_length);
	}
	if()
}