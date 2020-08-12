#include "../util.cpp"
#include <fstream>

class RSA 
{   
    public: 
    RSA();
    ~RSA();
    std::string RSAgetmessage(std::string cryptotext);
    std::string RSAgetcryptotext(std::string message);
    void SaveKeys(std::string filename);
    void LoadKeys(std::string filename);
    void SetKeys(std::string e2_, std::string N2_);
    void getPublicKeys(mpz_t pub1, mpz_t pub2);
    //your public key
    mpz_t e;
    mpz_t N;
    
    private: 
    //other party's public key
    mpz_t e2;
    mpz_t N2;
    
    //private key (both the same key different types for operations)
    mpf_t d; //float 
    mpz_t new_d; //integer

    //messages
    mpz_t m;//to send
    mpz_t m2;//received and decrypted

    //cryptotext
    mpz_t c;//to send and encrypted
    mpz_t c2;//received

    void RSAEncrypt(std::string message);
    void RSADecrypt(std::string cryptotext);
    void RSAKeyGen();    

}; 
