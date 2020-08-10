#include "RSA.h"

//init
RSA::RSA(){
	mpz_init(e);
	mpz_init(N);
	mpz_init(e2);
	mpz_init(N2);
	mpf_init(d);
	mpz_init(new_d);
	mpz_init(m);
	mpz_init(m2);
	mpz_init(c);
	mpz_init(c2);
	RSAKeyGen();
}

//desc: generates public and private keys
void RSA::RSAKeyGen() {
	//Generate 2 random prime numbers
	mpz_t p; 
	mpz_t q;
	mpz_init(p);
	mpz_init(q);
	getRandPrime(p);
	getRandPrime(q);
	mpz_mul(N,p,q);

	mpz_t carmichaelNum;
	mpz_init(carmichaelNum);
	calcLCM(carmichaelNum, p - 1, q - 1);
	
	mpf_t ef;
	mpf_init(ef);
	mpf_t k;
	mpf_init(k);
	mpf_set_si(k,1);
	mpf_t carmNumf;
	mpf_init(carmNumf);
	mpf_set_z(carmNumf,carmichaelNum);

	mpf_t d;
	mpf_init(d);
	mpf_add(carmNumf,carmNumf,k);
	mpf_div(d,carmNumf,ef);
	mpf_t tmp;
	mpf_init(tmp);
	mpf_floor(tmp,d);
	while (!mpf_cmp(d,tmp)) {
		mpf_add(carmNumf,carmNumf,k);
		mpf_div(d,carmNumf,ef);
		mpf_floor(tmp,d);
	}
	mpz_set_f(new_d,d);
	return;
}

//desc: encrypts message using RSA
void RSA::RSAEncrypt(std::string message) {
	//convert message to mpz_t and store in m
	const char* msg = message.c_str();
	mpz_set_str(m,msg,10);
	pow(c, m, e2, N2);
	return;
}


//desc: decrypts message using RSA
void RSA::RSADecrypt(std::string cryptotext) {
	//convert cryptotext to mpz_t and store in c2
	const char* ctxt = cryptotext.c_str();
	mpz_init_set_str(c2,ctxt,10);
	pow(m2, c2, new_d, N);
	return;
}

//returns decrypted message
std::string RSA::RSAgetmessage(){
	//convert c2 to string
	mpz_class ctxt(c2);
	std::string cryptotext = ctxt.get_str();
	RSADecrypt(cryptotext);
	//convert m2 to string
	mpz_class msg(m2);
	std::string message = msg.get_str();
	return message;
}

//returns encrypted message
std::string RSA::RSAgetcryptotext(){
	//convert m to string
	mpz_class msg(m);
	std::string message = msg.get_str();
	RSAEncrypt(message);
	//convert c to string
	mpz_class ctxt(c);
	std::string cryptotext = ctxt.get_str();
	return cryptotext;
}