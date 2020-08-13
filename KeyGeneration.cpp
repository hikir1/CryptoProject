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
void KeyGen::KeyExchange_client(mpz_t sendingkeyhalf, mpz_t p, mpz_t g, mpz_t pkb)
{
	//establish public keys
	mpz_init(sendingkeyhalf);
	getRandPrime(p);
	getRandPrime(g);
	std::string priv;
	getRandPrime(pkb);
	sendKeyDiffieHellman(sendingkeyhalf,g,p,pkb);
}

void KeyGen::KeyExchange_server(mpz_t sendingkeyhalf, mpz_t p, mpz_t g, mpz_t pkb)
{
	mpz_init(sendingkeyhalf);
	//establish public keys
	std::string priv;
	getRandPrime(pkb);
	sendKeyDiffieHellman(sendingkeyhalf,g,p,pkb);
}

//output vector -> Keyhalf , p , g, pkb
std::vector<std::string> KeyGen::createKeyhalf_client(){
	std::vector<std::string> res;

	mpz_t keyhalf;
	mpz_init(keyhalf);

	mpz_t p;
	mpz_init(p);

	mpz_t g;
	mpz_init(g);

	mpz_t pkb;
	mpz_init(pkb);

	mpz_t other_keyhalf;
	mpz_init(other_keyhalf);

	mpz_t shared_key;
	mpz_init(shared_key);

	KeyExchange_client(keyhalf,p,g,pkb); //keyhalf has proper values after this
	mpz_class tmp(keyhalf);
	mpz_class p_str(p);
	mpz_class g_str(g);
	mpz_class priv_key_str(pkb);
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
	res.push_back(p_str.get_str());
	res.push_back(g_str.get_str());
	res.push_back(priv_key_str.get_str());
	return res;
}

std::vector<std::string> KeyGen::createKeyhalf_server(std::string p, std::string g){
	std::vector<std::string> res;

	mpz_t keyhalf;
	mpz_init(keyhalf);

	mpz_t pkb;
	mpz_init(pkb);
	
	mpz_t pz;
	mpz_init(pz);

	mpz_t gz;
	mpz_init(gz);

	string2mpzt(pz,p);
	string2mpzt(gz,g);

	KeyExchange_server(keyhalf,pz,gz,pkb); //keyhalf has proper values after this
	mpz_class tmp(keyhalf);

	mpz_class p_str(p);
	mpz_class g_str(g);
	mpz_class priv_key_str(pkb);
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
	res.push_back(p_str.get_str());
	res.push_back(g_str.get_str());
	res.push_back(priv_key_str.get_str());
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
	string2mpzt(pkb,res[3]);
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

void TestKeyExchange(){
	std::vector<std::string> keys = KeyGen::createKeyhalf_client();//generates key half to send as well as public key (p,g) and private key - client
	std::vector<std::string> keys2 = KeyGen::createKeyhalf_server(keys[1],keys[2]); // takes input from client's public key (p,g) and generates keyhalf to send and private key - server
	std::string shared = KeyGen::getSharedKey(keys,keys2[0]); // takes output vector from last step as input as well as received key half - client
	std::string shared2 = KeyGen::getSharedKey(keys2,keys[0]); // takes output vector from last step as input as well as received key half - server
	if(shared == shared2){
			std::cout << "Keys Matched!" <<std::endl;
	}
	else{
		std::cout<<shared2<<std::endl<<std::endl;
		std::cout<<shared<<std::endl;
	}
}

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
