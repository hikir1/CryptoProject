// KeyGeneration.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <math.h>
#include "util.cpp"

/*
input:	g,p - public keys 
		pkb - private key
		sendingkeyhalf - result of key to be sent stored here
desc: sends first key half of diffie hellman 
*/
void sendKeyDiffieHellman(mpz_t sendingkeyhalf, mpz_t g, mpz_t p, mpz_t pkb) {
	pow(sendingkeyhalf, g, pkb, p);
}

/*
input:	g,p - public keys
		sendingkeyhalf - result of key to be sent stored here
desc: sends first key half ( private key randomly generated )
*/
void sendKeyDiffieHellman(mpz_t sendingkeyhalf, mpz_t g, mpz_t p) {
	//generate random key to send
	mpz_t pkb;
	mpz_init(pkb);
	getRandPrime(pkb);
	pow(sendingkeyhalf, g, pkb, p);	
}

/*
input:	p - public key
		receivedkeyhalf - key half sent by other party
		sharedsecret - result of shared key stored here
desc: computes shared key ( private key randomly generated )
*/
void sharedkey(mpz_t sharedSecret, mpz_t receivedkeyhalf, mpz_t p){
	/*remove bottom 4 lines when socket prog is added*/
	const char * const constkey = "12345678901234567890";
	int err = mpz_set_str(receivedkeyhalf, constkey, 10); //err == 0 if pass
	//generate secret keys upon receiving
	pow(sharedSecret,receivedkeyhalf, pkb, p);
}

/*
input:	e - public key
		N - also public key but computed within function
		new_d - private key generated within function 
desc: encrypts
*/
void RSAKeyGen(mpz_t e, mpz_t N, mpz_t new_d) {
	//Generate 2 random prime numbers
	mpz_t p; 
	mpz_t q;
	mpz_init(p);
	mpz_init(q);
	getRandPrime(p);
	getRandPrime(q);
	mpz_mul(N,p,q);

	mpz_t carmichaelNum;
	mpz_init(carmichaelNum);
	calcLCM(carmichaelNum, p - 1, q - 1);
	
	mpf_t ef;
	mpf_init(ef);
	mpf_t k;
	mpf_init(k);
	mpf_set_si(k,1);
	mpf_t carmNumf;
	mpf_init(carmNumf);
	mpf_set_z(carmNumf,carmichaelNum);

	mpf_t d;
	mpf_init(d);
	mpf_add(carmNumf,carmNumf,k);
	mpf_div(d,carmNumf,ef);
	mpf_t tmp;
	mpf_init(tmp);
	mpf_floor(tmp,d);
	while (!mpf_cmp(d,tmp)) {
		mpf_add(carmNumf,carmNumf,k);
		mpf_div(d,carmNumf,ef);
		mpf_floor(tmp,d);
	}
	mpz_set_f(new_d,d);
	//SEND public key to server
	//RECV public key from server
	return;
}

void KeyExchange()
{
	int alg;
	std::cout << "Choose an algorithm\n";
	std::cout << "Enter [1] for Static Diffie Hellman\nEnter [2] for Ephemeral Diffie Hellman\n\n";
	std::cin >> alg;
	if (alg == 1) {
		//establish public keys
		//this will be done by server
		mpz_t p; 
		mpz_t g;
		mpz_init(p);
		mpz_init(g);
		getRandPrime(p);
		getRandPrime(g);
		std::string priv;
		std::cout << "Enter your private key: "<<std::endl;
		std::cin >> priv;
		mpz_t pkb;
		mpz_init(pkb);
		mpz_set_str(pkb,priv,10);
		staticdiffieHellman(sharedsecret,g,p,pkb);
	}
	else if (alg == 2) {
		//establish public keys
		//this will be done by server
		mpz_t p; 
		mpz_t g;
		mpz_init(p);
		mpz_init(g);
		getRandPrime(p);
		getRandPrime(g);
		return 0;
	}
	else {
		std::cout << "Error";
		std::cerr << "Invalid Algorithm Option: " << alg;
		return -1;
	}

}
