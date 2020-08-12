#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string> 
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <gmpxx.h>
#include <stdexcept>
#include "aes/aes.hpp"
#include "hmac/hmac.h"
#include "ssh.hpp"
#include "RSA/RSA.h"
#include "KeyGen.hpp"

int make_client(char * host, char * port) {
  // try to find HOST and PORT
  int client, value;
  struct addrinfo hints = {0};
  hints.ai_family = AF_UNSPEC;
  //set to TCP
  hints.ai_socktype = SOCK_STREAM;
  struct addrinfo * res, * itr;
  if ((value = getaddrinfo(host, port, &hints, &res)) != 0) {
    perror("Error: getaddrinfo failed");
    return -1;
  }
  itr = res;
  int sd;
  //loop through every socket and connect to the first one
  while(itr) {
    if ((client = socket(res->ai_family, res->ai_socktype, 0)) == -1) {
	  itr = itr->ai_next; // <<<<<<<<<<<<<<< added here
        continue;
    }
    if (connect(client, res->ai_addr, res->ai_addrlen) == -1) {
        close(client);
	  itr = itr->ai_next; // <<<<<<<<<<<<<< and here
        continue;
    }
    // <<<<< removed `itr = itr->ai_next` from here
    //	(this would make itr NULL on success, causing failure below)
    break;
  }
  freeaddrinfo(res); // all done with this structure
  if (itr == NULL) {
    perror("Error: client failed to connect");
    return -1;
  }
  return client;
}

int estab_con(int client, ssh::Keys all_keys, RSA my_rsa){
  int num_bytes;
  char buf[ssh::RECV_MAX];
  std::string msg(ssh::HELLO_MSG,strlen(ssh::HELLO_MSG));
  std::cout << "check" << std::endl;
  std::string encrypted_msg = my_rsa.RSAgetcryptotext(msg);
   std::cout << "check" << std::endl;
  //send message
  int fail = write( client, msg.c_str(), msg.length()); 
  if ( fail < msg.length() ){
    perror( "write() failed" );
    return -1;
  }
  //get message
  if ((num_bytes = recv(client, buf, ssh::RECV_MAX-1, 0)) == -1) {
    perror("Error: recv failed");
    return -1;
  }
  buf[num_bytes] = '\0';
  // RSA Decrypt
  std::string received_msg(buf,strlen(buf));
  std::string decrypted_msg = my_rsa.RSAgetmessage(received_msg);
  if (msg.compare(decrypted_msg) != 0){
    perror( "Hacker detected" );
    return -1;
  }
  
  int broken = 0;
  for(int x = 0; x < 3; x++){
    mpz_t shared_key;
    mpz_init(shared_key);
    char hold[ssh::RECV_MAX];
    mpz_t keyhalf;
    mpz_init(keyhalf);
    mpz_t p;
    mpz_init(p);
    mpz_t pkb;
    mpz_init(pkb);
    mpz_t other_key_half;
    mpz_init(other_key_half);
    KeyGen::KeyExchange(keyhalf, p, pkb); //keyhalf has proper values after this
    mpz_class ctxt(keyhalf);
    std::string cryptotext = ctxt.get_str();
    //send keyhalf here
    fail = write( client, cryptotext.c_str(), cryptotext.length()); 
    if ( fail < cryptotext.length() ){
      perror( "write() failed" );
      return -1;
    }
    if ((num_bytes = recv(client, hold, ssh::RECV_MAX-1, 0)) == -1) {
      perror("Error: recv failed");
      return -1;
    }
    hold[num_bytes] = '\0';
    //receive key as char*
    mpz_set_str(other_key_half, hold, strlen(hold)); // this converts char* to key half
    KeyGen::sharedkey(shared_key, other_key_half, p, pkb); //stores shared key in shared_key after this
    if(x == 0){
      mpz_class var(shared_key);
      cryptotext = var.get_str();
      all_keys.hmac_key = cryptotext;
    }else if(x == 1){
      broken = aes::fill_key(all_keys.aes_key, shared_key);
      if(broken != 0){
        perror("Error: fill_key failed");
        return -1;
      }
    }else if(x == 2){
      broken = aes::fill_iv(all_keys.aes_iv, shared_key);
      if(broken != 0){
        perror("Error: fill_key failed");
        return -1;
      }
    }
  }
  return client;
}

int main(int argc, char ** argv)
{
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <HOST> <PORT>" << std::endl;
    return EXIT_FAILURE;
  }
  ssh::Keys all_keys;
  //set connection
  //confirm connection
  //key
  int num_bytes, fail;
  char buf[ssh::RECV_MAX];
  int client = make_client(argv[1], argv[2]);
  if (client == -1)
    return EXIT_FAILURE;
  // RSA Encrypt
  RSA my_rsa;
  my_rsa.LoadKeys("clientKeys");
  //send public keys
  //receive keys
  //make keys
  std::string message;
  while(1){
    int test = estab_con(client, all_keys, my_rsa);
    if(test == -1){
      return EXIT_FAILURE;
    }
    std::cout << "For depositing funds please use the format \"1 $amount\"" << std::endl;
    std::cout << "For withdrawing funds please use the format \"2 $amount\"" << std::endl;
    std::cout << "To deplay your balance please use the format \"3\"" << std::endl;
    std::cout << "To leave please uses \"q\"" << std::endl << std::endl;
    char first[ssh::RECV_MAX];
    char last[ssh::RECV_MAX];
    std::cout << "Enter your transaction below:" << std::endl;
    if (!std::getline(std::cin, message)) {
      perror("Error: getline failed");
      return EXIT_FAILURE;
    }
    if(message.length() == 0){
      std::cout << "Message Aborted: Message was too short" << std::endl;
      continue;
    }
    else if(message[0] == 'q'){
      std::cout << "Bye" << std::endl;
      break;
    }

    // if length (ssh) msg_max is exceeded abort
    if (message.length() > ssh::RECV_MAX){
        std::cout << "Message Aborted: Message was too long" << std::endl;
        continue;
    }
    // checks for the initial character
    // 1 = deposit (ssh)
    // 2 = withdraw (ssh)
    // 3 = balance (ssh)
    //ignore if message doesn't start with one of these 3
    else if (message[0] != (int)ssh::MsgType::DEPOSIT && message[0] != (int)ssh::MsgType::WITHDRAW && message[0] != (int)ssh::MsgType::BALANCE){
        std::cout << "Message Aborted: Message had improper start" << std::endl;
        continue;
    }
    // checks for $
    else if (message[2] != '$'){
        std::cout << "Message Aborted: No $ detected" << std::endl;
        continue;
    }
    std::string checker = "";
    for(int x = 2; x < message.length(); x++){
      checker = checker+message[x];
    }
    try{
      unsigned long long int money = stoull(checker);
    }catch(const std::invalid_argument& ia){
       std::cerr << "Invalid arguments: " << ia.what() << std::endl;
       std::cerr << "Message Aborted: Cannot be converted" << std::endl;
       continue;
    }catch(const std::out_of_range& oor){
       std::cerr << "Invalid arguments: " << oor.what() << std::endl;
       std::cerr << "Message Aborted: amount too large" << std::endl;
       continue;
    }
    //feed buffer make sure there is at least blockbuffer extra space
    //otherwise 
    //check if returned message.length%block_bytes == 0
    //if not abort
    //when message is recreved check it by decrypting message
    //then compare macs
    std::string mac = hmac::create_HMAC(message, all_keys.hmac_key);
    //send keyhalf here
    fail = write( client, mac.c_str(), mac.length()); 
    if ( fail <  mac.length() ){
      perror( "write() failed" );
      return EXIT_FAILURE;
    }
    const char *cstr = message.c_str();
    char holder[strlen(cstr) + aes::BLOCK_BYTES];
    size_t s_mess = strlen(cstr);
    aes::cbc_encrypt(cstr, holder, s_mess, all_keys.aes_iv, all_keys.aes_key);
    std::string encrypted(holder, strlen(holder));
    fail = write( client, encrypted.c_str(), encrypted.length()); 
    if ( fail < encrypted.length()){
      perror( "write() failed" );
      return EXIT_FAILURE;
    }
    if ((num_bytes = recv(client, first, ssh::RECV_MAX-1, 0)) == -1) {
      perror("Error: recv failed");
      return EXIT_FAILURE;
    }
    first[num_bytes] = '\0';
    std::string en_msg(first,strlen(first));
    if ((num_bytes = recv(client, last, ssh::RECV_MAX-1, 0)) == -1) {
      perror("Error: recv failed");
      return EXIT_FAILURE;
    }
    last[num_bytes] = '\0';
    // AES Decrypt
    char holder2[strlen(last) + aes::BLOCK_BYTES];
    if(en_msg.length()%aes::BLOCK_BYTES != 0){
      perror( "Hacker detected: incorrect length" );
      return EXIT_FAILURE;
    }
    s_mess = strlen(last);
    aes::cbc_decrypt(last, holder2, s_mess, all_keys.aes_iv, all_keys.aes_key);
    std::string gn_msg(holder2,strlen(holder2));
    std::string check = hmac::create_HMAC(gn_msg, all_keys.hmac_key);
    if(en_msg.compare(check) != 0){
      perror( "Hacker detected: incorrect HMAC" );
      return EXIT_FAILURE;
    }else{
      std::cout << gn_msg << std::endl;
    }
  }
  sleep(5);
  if (close(client) == -1) {
      perror("ERROR: Failed to close socket");
      return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
