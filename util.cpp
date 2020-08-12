#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <sstream>
#include <random>
#include <gmpxx.h>
#include <unordered_map>
#include <unistd.h>
#include <bits/stdc++.h>
#include <algorithm>
#include <stdio.h>
#include <tgmath.h>
#include <cstdint>
#include <fstream>
#include <iomanip>

//set static global so doesn't reinitialize on random calls
static std::random_device rd;
gmp_randstate_t rstate;
gmp_randinit_mt(rstate);

// Modular Exponentation 
void pow(mpz_t result, mpz_t x, mpz_t y, mpz_t p)  
{  
    mpz_powm(result,x,y,p); 
}

//calculate lcm of a and b
void calcLCM(mpz_t result, mpz_t a, mpz_t b) {
	mpz_lcm(result,a,b);
}

//check if N is prime
int isPrime(mpz_t N) {
	return mpz_probab_prime_p (N,50);
}

void getRandPrime(mpz_t result) {
	gmp_randseed_ui(rstate,rd());
    mpz_init(result);
    while(1){
		do{
	        mpz_urandomb(result, rstate, 128);
	    }while(mpz_cmp(result, rmin) < 0);
	    if (mpz_probab_prime_p(result, 50))
          break;
	}y
}
