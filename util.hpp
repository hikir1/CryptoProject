#ifndef _UTIL_HPP_
#define _UTIL_HPP_

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


std::string convertFromASCII(std::string str);
std::string convertToASCII(std::string message);

std::string mpzt2string(mpz_t a);
void string2mpzt(mpz_t res, std::string op);

//set static global so doesn't reinitialize on random calls
extern std::random_device rd;

// Modular Exponentation 
void pow(mpz_t result, mpz_t x, mpz_t y, mpz_t p);  

//calculate lcm of a and b
void calcLCM(mpz_t result, mpz_t a, mpz_t b);

//check if N is prime
int isPrime(mpz_t N);

void getRandPrime(mpz_t result);

#endif
