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
	mpz_t carmichaelNum;
	mpz_init(carmichaelNum);
	calcLCM(carmichaelNum, p , q);
	mpz_clear(p);
	mpz_clear(q);

	mpz_init(e);
	getRandPrime(e);
	mpf_t ef;
	mpf_init(ef);
	mpf_set_z(ef,e);
	mpf_t k;
	mpf_init(k);
	mpf_set_si(k,1);
	mpf_t carmNumf;
	mpf_init(carmNumf);
	mpf_set_z(carmNumf,carmichaelNum);
	mpz_clear(carmichaelNum);

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
	mpf_clear(tmp);
	mpf_clear(carmNumf);
	mpf_clear(ef);
	mpf_clear(k);
	mpz_set_f(new_d,d);
	mpf_clear(d);
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
   	inFile >> x;
    mpz_set_str(new_d,x.c_str(),10);
   	inFile >> x;
    mpz_set_str(N2,x.c_str(),10);
   	inFile >> x;
    mpz_set_str(e2,x.c_str(),10);
    inFile >> x;
    mpz_set_str(N,x.c_str(),10);
   	inFile >> x;
    mpz_set_str(e,x.c_str(),10);
    inFile.close();
	return;
}

int main(int argc, char ** argv){
	std::cout<<"Testing RSA Key Writing" <<std::endl;
	RSA myRSA;
	myRSA.SetKeys("1231424", "2354264523");
	std::string test = myRSA.RSAgetcryptotext("143252213");
	myRSA.SaveKeys("keys.txt");
	RSA myRSA2;
	myRSA2.SetKeys("12313643463", "124235");
	myRSA2.LoadKeys("keys.txt");
	myRSA2.SaveKeys("keys2.txt");
	return 0;
}
