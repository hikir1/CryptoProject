// KeyGeneration.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <math.h>
#include "util.cpp"

//diffie hellman approach with predefined key values
//both users run this to get shared secret
void staticdiffieHellman(mpz_t sharedSecret, mpz_t g, mpz_t p, mpz_t pkb) {
	//CODE TO SEND KEY HERE
	mpz_t sendingkeyhalf;
	mpz_init(sendingkeyhalf);


	pow(sendingkeyhalf, g, pkb, p);

	//CODE TO RECEIVE GENERATED KEY HERE
	
	/*remove bottom 4 lines when socket prog is added*/
	mpz_t receivedkeyhalf;
	mpz_init(receivedkeyhalf);
	const char * const constkey = "12345678901234567890";
	int err = mpz_set_str(receivedkeyhalf, constkey, 10); //err == 0 if pass
	

	//generate secret keys upon receiving
	pow(sharedSecret,receivedkeyhalf, pkb, p);
}

//diffie hellman approach with changing key values with each message
//both users run this to get shared secret
void ephemeralDiffieHellman(mpz_t sharedSecret, mpz_t g, mpz_t p) {
	//generate random key to send
	mpz_t sendingkeyhalf;
	mpz_init(sendingkeyhalf);

	mpz_t pkb;
	mpz_init(pkb);
	getRandPrime(pkb);

	pow(sendingkeyhalf, g, pkb, p);
	//CODE TO SEND KEY HERE

	//CODE TO RECEIVE GENERATED KEY HERE
	/*remove bottom 4 lines when socket prog is added*/
	mpz_t receivedkeyhalf;
	mpz_init(receivedkeyhalf);
	const char * const constkey = "12345678901234567890";
	int err = mpz_set_str(receivedkeyhalf, constkey, 10); //err == 0 if pass
	

	//generate secret keys upon receiving
	pow(sharedSecret,receivedkeyhalf, pkb, p);
}


//e is the public key
//
void RSAKeyGen(mpz_t e, mpz_t N, mpz_t N2, mpz_t new_d) {
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
	//SEND e to server
	//Receive new e from server
	//SEND N to server
	//Receive new N (N2) from server
	return;
}

	/*
int main()
{


	int alg;
	std::cout << "Choose an algorithm\n";
	std::cout << "Enter [1] for Static Diffie Hellman\nEnter [2] for Ephemeral Diffie Hellman\n\n";
	std::cin >> alg;
	if (alg == 1) {
		return 0;
	}
	else if (alg == 2) {
		return 0;
	}
	else {
		std::cout << "Error";
		std::cerr << "Invalid Algorithm Option: " << alg;
		return -1;
	}

}
	*/