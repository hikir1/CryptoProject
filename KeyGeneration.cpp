// KeyGeneration.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <bitset>
#include <vector>
#include <string>
#include <bitset>
#include <sstream>
#include <chrono>
#include <random>
#include <math.h>
// Power function to return value of a ^ b mod P 
unsigned long long pow(unsigned long long a, unsigned long long b,
	unsigned long long P)
{
	if (b == 1)
		return a;

	else
		return (((unsigned long long)pow(a, b)) % P);
}

//calculate gcd of a and b
unsigned long long calcGCD(unsigned long long a, unsigned long long h) {
	unsigned long long tmp;
	while (1) {
		tmp = a % h;
		if (tmp == 0) {
			return h;
		}
		a = h;
		h = tmp;
	}
}

//calculate lcm of a and b
unsigned long long calcLCM(unsigned long long a, unsigned long long b) {
	// maximum value between a and b is stored in max
	unsigned long long max = (a > b) ? a : b;
	do
	{
		if (max % a == 0 && max % b == 0)
		{
			return max;
		}
		else
			++max;
	} while (true);

	return 0;
}

//check if N is prime
int isPrime(unsigned long long N) {
	if (N < 2 || (!(N & 1) && N != 2))
		return 0;
	for (int i = 3; i*i <= N; i += 2) {
		if (!(N%i))
			return 0;
	}
	return 1;
}

unsigned long long getRandPrime() {
	while (true) {
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

		std::mt19937 generator(seed);  // mt19937 is a standard mersenne_twister_engine
		unsigned long long r = generator();
		if (isPrime(r)) {
			return r;
		}
	}
}

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


std::string convert_binary(std::string swapped, int *num) {
	//takes in a string and converts it to binary
	std::string full_binary = "";
	//loops through each character and turns its ascii value
	//and turns it into 
	for (unsigned int x = 0; x < swapped.length(); x++) {
		int A_value = int(swapped[x]);
		std::string part_binary = "";
		//turns the ascii value into binary
		while (A_value > -1) {
			(A_value % 2) ? part_binary.push_back('1') : part_binary.push_back('0');
			//catches if the first binary bit is zero
			if (A_value == 0) {
				break;
			}
			A_value /= 2;
		}
		reverse(part_binary.begin(), part_binary.end());
		//adds it to the overall binary
		full_binary = full_binary + part_binary;
	}
	return full_binary;
}

std::string convert_plaintext(std::string answer) {
	std::string plaintext = "";
	//creates a stringstream pointing to answer
	std::stringstream sstream(answer);
	//while there are no error states
	while (sstream.good())
	{
		//find 8 bits in the stream
		std::bitset<8> bits;
		sstream >> bits;
		//form a char from them
		char character = char(bits.to_ulong());
		//adds it to plaintext
		plaintext = plaintext + character;
	}
	return plaintext;
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