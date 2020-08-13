#include "RSA/RSA.h"
#include <iostream>

int main(){
	RSA clientRSA;
	RSA serverRSA;

	clientRSA.RSAKeyGen();
	serverRSA.RSAKeyGen();

	mpz_t client_e;
	mpz_t server_e;
	mpz_t client_N;
	mpz_t server_N;

	mpz_init(client_e);
	mpz_init(server_e);
	mpz_init(client_N);
	mpz_init(server_N);

	clientRSA.getPublicKeys(client_e,client_N);
	serverRSA.getPublicKeys(server_e,server_N);

	mpz_class c_e(client_e);
	mpz_class serv_e(server_e);
	mpz_class c_N(client_N);
	mpz_class serv_N(server_N);
	
	clientRSA.SetKeys(serv_e.get_str(),serv_N.get_str());
	serverRSA.SetKeys(c_e.get_str(),c_N.get_str());

	clientRSA.SaveKeys("clientKeys");
	serverRSA.SaveKeys("serverKeys");

gmp_printf(":::::: client: %Zd %Zd\n", clientRSA.e, clientRSA.N);
gmp_printf(":::::: server: %Zd %Zd\n", serverRSA.e, serverRSA.N);

	mpz_clear(client_e);
	mpz_clear(server_e);
	mpz_clear(client_N);
	mpz_clear(server_N);
}
