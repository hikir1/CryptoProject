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

int Bin_to_Dec(long long int given){
	//takes in a long long int of binary converts it to decimal
    int bin_answer = 0;
    int count = 0;
    int remainder = 0;
    //changes it from base 2
    while (given != 0){
        remainder = given % 10;
        given = given / 10;
        bin_answer = bin_answer + remainder * pow(2,count);
        count++;
    }
    return bin_answer;
}


long long int Dec_to_Bin(int given){
	//takes in decimal converts it to binary
    long long int num_answer = 0;
    int remainder = 0; 
    int count = 1;
    //changes it into base 2
    while (given != 0){
        remainder = given % 2;
        given = given / 2;
        num_answer = num_answer + remainder * count;
        count = count * 10;
    }
    return num_answer;
}

string convert_binary(string swapped){
	//takes in a string and converts it to binary
	string full_binary = "";
	string part_binary = "";
	//loops through each character and turns its ascii value
	//and turns it into 
    for (unsigned int x = 0; x < swapped.length(); x++){
		part_binary = bitset<8>(swapped.c_str()[x]).to_string();
        full_binary = full_binary + part_binary;
    }
    return full_binary;
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