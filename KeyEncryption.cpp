#include "KeyGeneration.cpp"

void RSAEncrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t N2) {
	return pow(c, m, e, N2);
	return;
}

void RSADecrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t N) {
	pow(m, c, d, N);
	return;
}

int main()
{
	std::cout << "done" <<std::endl;
	return 1;
}