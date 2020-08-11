#ifndef KEY_GEN_H_
#define KEY_GEN_H_

#include <math.h>
#include "util.hpp"

namespace KeyGen {
    extern int diffiekeyhalfsize;
    void sendKeyDiffieHellman(mpz_t sendingkeyhalf, mpz_t g, mpz_t p, mpz_t pkb);
    void sendKeyDiffieHellman(mpz_t sendingkeyhalf, mpz_t g, mpz_t p);
    void sharedkey(mpz_t sharedSecret, mpz_t receivedkeyhalf, mpz_t p, mpz_t pkb);
    void KeyExchange(mpz_t sendingkeyhalf, mpz_t p, mpz_t pkb);
}; 

#endif
