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
}

//destructor
RSA::~RSA(){
	mpz_clear(e);
	mpz_clear(N);
	mpz_clear(e2);
	mpz_clear(N2);
	mpf_clear(d);
	mpz_clear(new_d);
	mpz_clear(m);
	mpz_clear(m2);
	mpz_clear(c);
	mpz_clear(c2);
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
	mpz_sub_ui(p,p,1);
	mpz_sub_ui(q,q,1);
	mpz_t toitent;
	mpz_init(toitent);
	mpz_mul(toitent,p,q);
	mpz_clear(p);
	mpz_clear(q);
    mpz_set_ui(e,65537);
 	mpz_t count;
 	mpz_init(count);
    //for checking co-prime which satisfies e>1
    while(mpz_cmp(e,toitent) <= 0){
    
	    mpz_gcd(count,e,toitent);
	    if(mpz_cmp_ui(count,1) == 0){
	        break;
	    }
	    else{
	        mpz_add_ui(e,e,1);
	    }
    }
    mpz_invert(new_d,e,toitent);
	mpz_clear(toitent);
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
	mpz_set_str(c2,ctxt,10);
	pow(m2, c2, new_d, N);
	return;
}

//returns decrypted message
std::string RSA::RSAgetmessage(std::string cryptotext){
	RSADecrypt(cryptotext);
	//convert m2 to string
	mpz_class msg(m2);
	std::string message = msg.get_str();
	return message;
}

//returns encrypted message
std::string RSA::RSAgetcryptotext(std::string message){
	RSAEncrypt(message);
	//convert c to string
	mpz_class ctxt(c);
	std::string cryptotext = ctxt.get_str();
	return cryptotext;
}

//set other party's public key
void RSA::SetKeys(std::string e2_, std::string N2_){
	const char* new_e2 = e2_.c_str();
	const char* new_n2 = N2_.c_str();
	mpz_set_str(e2,new_e2,10);
	mpz_set_str(N2,new_n2,10);
}

//writes file containing own private key and other's public key
void RSA::SaveKeys(std::string filename){
	std::ofstream of(filename);
	 if(of.is_open())
    {	
    	mpz_class priv(new_d);
    	mpz_class pub1(N2);
    	mpz_class pub2(e2);
    	mpz_class mypub1(N);
    	mpz_class mypub2(e);
    	std::string privatekey = priv.get_str();
    	std::string publickey1 = pub1.get_str();
    	std::string publickey2 = pub2.get_str(); 
		std::string mypublickey1 = mypub1.get_str();
    	std::string mypublickey2 = mypub2.get_str(); 
        of<< privatekey << std::endl << publickey1 << std::endl << publickey2 <<std::endl << mypublickey1 << std::endl << mypublickey2 <<std::endl;
        of.flush();
        of.close();
    }
    else
    {
        std::cerr <<"Failed to open file : "<<std::endl;
        return;
    }
	return;
}

//gets public key
void RSA::getPublicKeys(mpz_t pub1, mpz_t pub2){
	mpz_set(pub1,e);
	mpz_set(pub2,N);
}

//gets other party'spublic key
void RSA::getOtherPublicKeys(mpz_t pub1, mpz_t pub2){
	mpz_set(pub1,e2);
	mpz_set(pub2,N2);
}

//loads on other's public key from file
void RSA::LoadKeys(std::string filename){
	std::string x;
	std::ifstream inFile(filename);
    
    if (!inFile) {
        std::cerr << "Unable to open file";
        exit(1); // terminate with error
    }
    mpz_clear(new_d);
    mpz_clear(N2);
    mpz_clear(e2);
    mpz_init(new_d);
    mpz_init(N2);
    mpz_init(e2);
    mpz_init(N);
   	inFile >> x;
    mpz_set_str(new_d,x.c_str(),10);
   	inFile >> x;
    mpz_set_str(N2,x.c_str(),10);
   	inFile >> x;
    mpz_set_str(e2,x.c_str(),10);
    inFile >> x;
    mpz_set_str(N,x.c_str(),10);
    inFile.close();
	return;
}

int main(int argc, char ** argv){
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
	std::cout<< "Server N: " <<serv_N.get_str() <<std::endl;
	std::cout<< "Client N: " <<c_N.get_str() <<std::endl;

	clientRSA.SaveKeys("clientKeys");
	serverRSA.SaveKeys("serverKeys");
	clientRSA.LoadKeys("clientKeys");
	serverRSA.LoadKeys("serverKeys");
	//encrypt message
	std::string ctxt = serverRSA.RSAgetcryptotext("123456789012345678901234567891234567890123456789012345678901234567890123456789012345678901234567890");

	//decrypt message
	std::string msg = clientRSA.RSAgetmessage(ctxt);

	std::cout<<ctxt<<std::endl<<std::endl<<msg<<std::endl;
	return 0;
}
