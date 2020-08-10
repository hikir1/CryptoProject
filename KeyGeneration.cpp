// KeyGeneration.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <math.h>
#include "util.cpp"

const int diffiekeyhalfsize = 128;

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
		pkb - private key
		receivedkeyhalf - key half sent by other party
		sharedsecret - result of shared key stored here
desc: computes shared key ( private key randomly generated )
*/
void sharedkey(mpz_t sharedSecret, mpz_t receivedkeyhalf, mpz_t p, mpz_t pkb){
	/*remove bottom 4 lines when socket prog is added*/
	const char * const constkey = "12345678901234567890";
	int err = mpz_set_str(receivedkeyhalf, constkey, 10); //err == 0 if pass
	//generate secret keys upon receiving
	pow(sharedSecret,receivedkeyhalf, pkb, p);
}

/*
	input: sendingkeyhalf - empty initialized mpz_t 
*/
void KeyExchange(mpz_t sendingkeyhalf)
{
	int alg;
	std::cout << "Choose an algorithm\n";
	std::cout << "Enter [1] to input your own private key\nEnter [2] for randomly generated private key\n\n";
	std::cin >> alg;
	if (alg == 1) {
		//establish public keys
		mpz_t p; 
		mpz_t g;
		mpz_init(p);
		mpz_init(g);
		//getRandPrime(p);
		//getRandPrime(g);
		mpz_set_ui(p,23);
		mpz_set_ui(g,9);
		std::cout << "P: " << p <<std::endl << "G: " << g <<std::endl;
		//enter private key
		std::string priv;
		std::cout << "Enter your private key: "<< std::endl;
		std::cin >> priv;
		mpz_t pkb;
		mpz_init(pkb);
		const char* pk = priv.c_str();
		mpz_set_str(pkb,pk,10);
		std::cout << "Your key: "<< pkb <<std::endl;
		sendKeyDiffieHellman(sendingkeyhalf,g,p,pkb);
		mpz_clear(p);
		mpz_clear(g);
		std::cout << "Sending Key Half: " << sendingkeyhalf <<std::endl;
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
		sendKeyDiffieHellman(sendingkeyhalf,g,p);
	}
	else {
		std::cout << "Error" <<std::endl <<std::flush;
		std::cerr << "Invalid Algorithm Option: " << alg <<std::endl;
	}
}

int main(){
	//test diffie hellman
	mpz_t k;
	mpz_init(k);
	KeyExchange(k);
	mpz_t rkey;
	mpz_set_ui(rkey,16);
	mpz_t sharedsecret;
	std::cout<<"here"<<std::endl<<std::flush;
	mpz_t p;
	mpz_init(p);
	mpz_set_ui(p,23);
	mpz_init(sharedsecret);
	mpz_t pkb;
	mpz_init(pkb);
	mpz_set_ui(pkb,4);
	sharedkey(sharedsecret,rkey,p,pkb);
}