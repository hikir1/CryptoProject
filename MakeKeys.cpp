#include "RSA/RSA.cpp"

int main(){
	RSA clientRSA;
	RSA serverRSA;

	mpz_t client_e;
	mpz_t server_e;
	mpz_t client_N;
	mpz_t server_N;

	mpz_init(client_e);
	mpz_init(server_e);
	mpz_init(client_N);
	mpz_init(server_N);

	clientRSA.getpublicKeys(client_e,client_N);
	serverRSA.getpublicKeys(server_e,server_N);

	mpz_class(client_e);
	mpz_class(server_e);
	mpz_class(client_N);
	mpz_class(server_N);
	
	clientRSA.SetKeys(server_e,server_N);
	serverRSA.SetKeys(client_e,client_N);

	clientRSA.SaveKeys();
	serverRSA.SaveKeys();

	mpz_clear(client_e);
	mpz_clear(server_e);
	mpz_clear(client_N);
	mpz_clear(server_N);

}