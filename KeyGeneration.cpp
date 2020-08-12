// KeyGeneration.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <math.h>
#include "util.hpp"
#include "KeyGen.hpp"

int KeyGen::diffiekeyhalfsize = 128;


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
	/*remove bottom 4 lines when socket prog is added*/
	const char * const constkey = "1234567890";
	int err = mpz_set_str(receivedkeyhalf, constkey, 10); //err == 0 if pass
	//generate secret keys upon receiving
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