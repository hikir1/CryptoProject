#include "KeyGeneration.cpp"

RSAEncrypt(m, e, N2) {
	return pow(m, e, N2);
}

RSADecrypt(c, d, N) {
	return pow(c, d, N);
}

int main()
{
	std::cout << "Starting\n";
	std::cout << KeyGeneration.ephemeralDiffieHellman(2131421,1241353);
}