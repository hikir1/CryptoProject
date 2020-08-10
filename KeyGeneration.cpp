// KeyGeneration.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <math.h>
#include "util.cpp"

//diffie hellman approach with predefined key values
//both users run this to get shared secret
unsigned long long staticdiffieHellman(unsigned long long g, unsigned long long p, unsigned long long pkb) {
	//CODE TO SEND KEY HERE
	//CODE TO RECEIVE GENERATED KEY HERE
	unsigned long long receivedkey = 14901228261374;

	//generate secret keys upon receiving
	unsigned long long sharedSecret = pow(receivedkey, pkb, p);

	return sharedSecret;
}

//diffie hellman approach with changing key values with each message
//both users run this to get shared secret
int ephemeralDiffieHellman(unsigned long long g, unsigned long long p) {

	//predefined key to send
	//GENERATE RANDOM 256 BIT NUM
	unsigned long long pkb = 24708522251123;

	//CODE TO SEND KEY HERE
	//CODE TO RECEIVE GENERATED KEY HERE
	unsigned long long receivedkey = 14901228261374;

	//generate secret keys upon receiving
	unsigned long long sharedSecret = pow(receivedkey, pkb, p);

	return sharedSecret;
}


//e is the public key
//
double RSAKeyGen(double e) {
	//Generate 2 random prime numbers
	unsigned long long p = getRandPrime();
	unsigned long long q = getRandPrime();

	unsigned long long N = p * q;
	unsigned long long carmichaelNum = calcLCM(p - 1, q - 1);
	int k = 1;
	double d = double(k + carmichaelNum) / e;
	while (d != floor(d)) {
		k += 1;
		d = double(k*carmichaelNum) / e;
	}
	d = unsigned long long(d);
	//SEND e to server
	//Receive new e from server
	//SEND N to server
	//Receive new N (N2) from server
	unsigned long long N2;

	return d, N2, e, N;
}


int main()
{

	/*
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
	*/
}