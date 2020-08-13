// KeyGeneration.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <math.h>
#include "util.hpp"
#include "KeyGen.hpp"

/*
input:	g,p - public keys 
		pkb - private key
		sendingkeyhalf - result of key to be sent stored here
desc: sends first key half of diffie hellman 
*/
void KeyGen::sendKeyDiffieHellman(mpz_t sendingkeyhalf, mpz_t g, mpz_t p, mpz_t pkb) {
	pow(sendingkeyhalf, g, pkb, p);
}

/*
input:	g,p - public keys
		sendingkeyhalf - result of key to be sent stored here
desc: sends first key half ( private key randomly generated )
*/
void KeyGen::sendKeyDiffieHellman(mpz_t sendingkeyhalf, mpz_t g, mpz_t p) {
	//generate random key to send
	mpz_t pkb;
	mpz_init(pkb);
	getRandPrime(pkb);
	pow(sendingkeyhalf, g, pkb, p);	
	mpz_clear(pkb);
}

/*
input:	p - public key
		pkb - private key
		receivedkeyhalf - key half sent by other party
		sharedsecret - result of shared key stored here
desc: computes shared key ( private key randomly generated )
*/
void KeyGen::sharedkey(mpz_t sharedSecret, mpz_t receivedkeyhalf, mpz_t p, mpz_t pkb){
	pow(sharedSecret,receivedkeyhalf, pkb, p);
}

/*
	input: sendingkeyhalf - empty initialized mpz_t 
*/
void KeyGen::KeyExchange(mpz_t sendingkeyhalf, mpz_t p, mpz_t pkb)
{
	int alg;
	//establish public keys
	mpz_t g;
	mpz_init(g);
	getRandPrime(p);
	getRandPrime(g);
	std::string priv;
	getRandPrime(pkb);
	sendKeyDiffieHellman(sendingkeyhalf,g,p,pkb);
	mpz_clear(g);
}

//output vector -> Keyhalf , p , pkb
std::vector<std::string> KeyGen::createKeyhalf(){
	std::vector<std::string> res;

	mpz_t keyhalf;
	mpz_init(keyhalf);

	mpz_t p;
	mpz_init(p);

	mpz_t pkb;
	mpz_init(pkb);

	mpz_t other_keyhalf;
	mpz_init(other_keyhalf);

	mpz_t shared_key;
	mpz_init(shared_key);

	KeyExchange(keyhalf,p,pkb); //keyhalf has proper values after this
	mpz_class tmp(keyhalf);
	mpz_class tmp2(p);
	mpz_class tmp3(pkb);
	std::string keyhlf;
	std::string appendzero = "0";
	keyhlf = tmp.get_str();
	while(keyhlf.length() < 128){
		while((keyhlf.length() + appendzero.length()) < 128){
			appendzero = appendzero + "0";
		}
		keyhlf = appendzero + keyhlf;
	}
	res.push_back(keyhlf);
	res.push_back(tmp2.get_str());
	res.push_back(tmp3.get_str());
	return res;
}

//input vector result from createKeyHalf, keyhald received from other party
//output string with sharedSecret
std::string KeyGen::getSharedKey(std::vector<std::string> res, std::string other_keyhalf){
	mpz_t sharedSecret;
	mpz_t receivedkeyhalf;
	mpz_t p;
	mpz_t pkb;

	mpz_init(sharedSecret);
	mpz_init(receivedkeyhalf);
	mpz_init(p);
	mpz_init(pkb);

	string2mpzt(receivedkeyhalf,other_keyhalf);
	string2mpzt(p,res[1]);
	string2mpzt(pkb,res[2]);
 	sharedkey(sharedSecret,receivedkeyhalf,p,pkb);
 	std::string appendzero = "0";
 	std::string secKey = mpzt2string(sharedSecret);
	while(secKey.length() < 128){
		while((secKey.length() + appendzero.length()) < 128){
			appendzero = appendzero + "0";
		}
		secKey = appendzero + secKey;
	}
 	return secKey;
}

/*
int main(){
	std::vector<std::string> keys = KeyGen::createKeyhalf();
	std::vector<std::string> keys2 = KeyGen::createKeyhalf();
	std::string shared = KeyGen::getSharedKey(keys,keys2[0]);
	std::string shared2 = KeyGen::getSharedKey(keys2,keys[0]);
	if(shared == shared2){
			std::cout << "pooped" <<std::endl;
	}
	else{
		std::cout<<shared2<<std::endl<<std::endl;
		std::cout<<shared<<std::endl;
	}
}
*/
/* How to use code above

//initialize 3 mpz_t types
mpz_t keyhalf;
mpz_init(keyhalf);

mpz_t p;
mpz_init(p);

mpz_t pkb;
mpz_init(pkb);

mpz_t other_keyhalf;
mpz_init(other_keyhalf);

mpz_t shared_key;
mpz_init(shared_key);

KeyExchange(keyhalf); //keyhalf has proper values after this
//send keyhalf here

//receive key as char*
mpz_set_str(received char*, other_key_half, 10) // this converts char* to key half

sharedkey(shared_key, other_key, p, pkb) //stores shared key in shared_key after this

//mpz_t is a pointer, run mpz_clear(mpt_t variablename) to free the memory
*/
