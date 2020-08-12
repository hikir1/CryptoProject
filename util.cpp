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

//mpz_t to string
std::string mpzt2string(mpz_t a){
    std::string res;
    mpz_class tmp(a);
    res = tmp.get_str();
    return res;
}

//string to mpz_t
void stringt2mpzt(mpz_t res, std::string op){
    const char* str = op.c_str();
    mpz_set_str(res,str,10);
}

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
    unsigned min_digits = 16;
    unsigned max_digits = 16;
    mpz_t rmin;
    mpz_init(rmin);
    mpz_ui_pow_ui(rmin, 10, min_digits-1);

    mpz_t rmax;
    mpz_init(rmax);
    mpz_ui_pow_ui(rmax, 10, max_digits);

    gmp_randstate_t rstate;
    gmp_randinit_mt(rstate);
    gmp_randseed_ui(rstate,rd());
    mpz_init(result);
    while(1){
        do{
            mpz_urandomm(result, rstate, rmax);
        }while(mpz_cmp(result, rmin) < 0);
        if (mpz_probab_prime_p(result, 50))
          break;
    }
    mpz_clear(rmax);
    mpz_clear(rmin);
    gmp_randclear(rstate);
}