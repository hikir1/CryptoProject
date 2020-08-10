#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <sstream>
#include <chrono>
#include <random>
#include <gmpxx.h>

//set static global so doesn't reinitialize on random calls
static std::random_device rd;

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

	unsigned min_digits = 6;
    unsigned max_digits = 9;
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

/*
int main(){
	mpz_t rando;
	getRandPrime(rando);
	std::cout << "Your Random Prime: " << rando << std::endl; 
	return 0;
}
*/