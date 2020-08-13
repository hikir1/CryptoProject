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
	getRandPrime(e);
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
	std::string appendzero = "0";
	while(cryptotext.length() < 256){
		while((cryptotext.length() + appendzero.length()) < 256){
			appendzero = appendzero + "0";
		}
		cryptotext = appendzero + cryptotext;
	}
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
    mpz_clear(N);
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

int RSA::GetSendingSize(){
	mpz_class tmp_N2(N2);
	return tmp_N2.get_str().length();
}

int RSA::GetReceivingSize(){
	mpz_class tmp_N(N);
	return tmp_N.get_str().length();
}

void RSA::printKeys(){
	mpz_class tmp1(new_d);
	mpz_class tmp2(N2);
	mpz_class tmp3(e2);
	mpz_class tmp4(N);

	std::cout<<"d: " << tmp1.get_str() << std::endl;
	std::cout<<"N2: " << tmp2.get_str() << std::endl;
	std::cout<<"e2: " << tmp3.get_str() << std::endl;
	std::cout<<"N: " << tmp4.get_str() << std::endl;
}

int main(int argc, char ** argv){
	RSA clientRSA;
	RSA serverRSA;

	//clientRSA.RSAKeyGen();
	//serverRSA.RSAKeyGen();
	clientRSA.LoadKeys("clientKeys");
	serverRSA.LoadKeys("serverKeys");
	

	//clientRSA.SaveKeys("clientKeys");
	//serverRSA.SaveKeys("serverKeys");

	//encrypt message

	std::string message("I AM NOT A HACKER",strlen("I AM NOT A HACKER") + 1);
	//std::cout << "message: " << message <<std::endl;
	std::string input = convertToASCII(message);
	//std::cout << "ASCII convert: " << input <<std::endl;
	std::string ctxt = serverRSA.RSAgetcryptotext(input);
	std::cout << "cipher: " << ctxt <<std::endl;
	//decrypt message

	std::string msg = clientRSA.RSAgetmessage(ctxt);
	//std::cout << "unASCII's text: " << msg <<std::endl;	
	ctxt = convertFromASCII(msg);
	std::cout << "original message: " << ctxt <<std::endl;
	input = convertToASCII(ctxt);
	ctxt = clientRSA.RSAgetcryptotext(input);
	std::cout << "cipher 2x: " << ctxt <<std::endl;
	msg = serverRSA.RSAgetmessage(ctxt);
	std::string orig_msg = convertFromASCII(msg);
	std::cout << "Orginal message 2x: " << orig_msg <<std::endl;

	//std::cout <<ctxt<<std::endl;
	//std::cout <<"here"<<std::endl;
	
	//std::cout<<ctxt.length()<<std::endl<<std::endl<<msg<<std::endl;
	return 0;
}
