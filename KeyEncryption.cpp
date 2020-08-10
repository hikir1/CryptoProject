#include "util.hpp"
/*
input:	c - cryptotext to be sent stored here
		m - message to be encrypted
		e2,N2 - other party's public key
desc: encrypts message using RSA
*/
void RSAEncrypt(mpz_t c, mpz_t m, mpz_t e2, mpz_t N2) {
	pow(c, m, e2, N2);
	return;
}

/*
input:	c - cryptotext to be decrypted
		m - message to be read stored here
		d - private key
		N - user's public key half
desc: encrypts message using RSA
*/
void RSADecrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t N) {
	pow(m, c, d, N);
	return;
}
