#ifndef KEY_GEN_H_
#define KEY_GEN_H_

#include <math.h>
#include "util.hpp"

namespace KeyGen {
    constexpr size_t diffiekeyhalfsize = 128;
    constexpr size_t diffiekeysize = diffiekeyhalfsize;
    constexpr int base = 10;
    void sendKeyDiffieHellman(mpz_t sendingkeyhalf, mpz_t g, mpz_t p, mpz_t pkb);
    void sendKeyDiffieHellman(mpz_t sendingkeyhalf, mpz_t g, mpz_t p);
    void sharedkey(mpz_t sharedSecret, mpz_t receivedkeyhalf, mpz_t p, mpz_t pkb);
    void KeyExchange_client(mpz_t sendingkeyhalf, mpz_t p,mpz_t g, mpz_t pkb);
    void KeyExchange_server(mpz_t sendingkeyhalf, mpz_t p,mpz_t g, mpz_t pkb);
    std::string getSharedKey(std::vector<std::string> res, std::string other_keyhalf);
    std::vector<std::string> createKeyhalf_client();
    std::vector<std::string> createKeyhalf_server(std::string p, std::string g);

}; 

#endif
