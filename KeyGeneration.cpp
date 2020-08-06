// KeyGeneration.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <vector>

// Power function to return value of a ^ b mod P 
long long int pow(long long int a, long long int b,
	long long int P)
{
	if (b == 1)
		return a;

	else
		return (((long long int)pow(a, b)) % P);
}

//calculate gcd of a and b
long long int calcGCD(long long int a, long long int h) {
	int tmp;
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
long long int calcLCM(long long int a, long long int b) {
	// maximum value between a and b is stored in max
	long long int max = (a > b) ? a : b;
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
int isPrime(int N) {
	if (N < 2 || (!(N & 1) && N != 2))
		return 0;
	for (int i = 3; i*i <= N; i += 2) {
		if (!(N%i))
			return 0;
	}
	return 1;
}

double getRandPrime() {
	while (true) {
		double r = rand();
		if (isPrime(r)) {
			return r;
		}
	}
}

//diffie hellman approach
//both users run this to get shared secret
int diffieHellman(long long int g, long long int p,
	long long int pk) {

	//generate keys to send
	long long int genKey = pow(g, pk, p);

	//CODE TO SEND KEY HERE
	//CODE TO RECEIVE GENERATED KEY HERE
	//send pka
	//received pkb
	long long int pkb;

	//generate secret keys upon receiving
	long long int sharedSecret = pow(pkb, pk, p);

	return sharedSecret;
}

double RSA(double k, double e, double msg) {
	//Generate 2 random prime numbers
	double p = getRandPrime();
	double q = getRandPrime();

	double N = p * q;
	double phi = (p - 1)*(q - 1);

	//e needs to be coprime and less than phi
	while (e < phi) {
		if (calcGCD(e, phi) == 1) {
			break;
		}
		else {
			e++;
		}
	}

	//Private Key d
	double d = (1 + (k *phi)) / e;
	 
	//cryptotext
	double c = pow(msg, e);
	c = fmod(c, N);
	return c;
}

int main()
{
    std::cout << "Hello World!\n"; 
}