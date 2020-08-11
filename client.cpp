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
#include "aes/aes.hpp"
#include "hmac/hmac.h"
#include "ssh.hpp"
#include "RSA/RSA.h"
#include "KeyGen.hpp"


int main(int argc, char ** argv)
{
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <HOST> <PORT>" << std::endl;
    return EXIT_FAILURE;
  }
  Keys all_keys;
  //set connection
  //confirm connection
  //key
  int client, num_bytes;
  char buf[MSG_MAX];
  struct addrinfo information{0}, *server_info, *p;
  int value;
  information.ai_family = AF_UNSPEC;
  //set to TCP
  information.ai_socktype = SOCK_STREAM;
  if ((value = getaddrinfo(argv[1], argv[2], &information, &server_info)) != 0) {
    perror("Error: getaddrinfo failed");
    return EXIT_FAILURE;
  }
  //loop through every socket and connect to the first one
  for(p = server_info; p != NULL; p = p->ai_next) {
    if ((client = socket(p->ai_family, p->ai_socktype,
        p->ai_protocol)) == -1) {
        continue;
    }
    if (connect(client, p->ai_addr, p->ai_addrlen) == -1) {
        close(client);
        continue;
    }
    break;
  }
  freeaddrinfo(server_info); // all done with this structure
  if (p == NULL) {
    perror("Error: client failed to connect");
    return EXIT_FAILURE;
  }

  // RSA Encrypt
  RSA my_rsa;
  std::string msg(HELLO_MSG,sizeof(HELLO_MSG));
  my_rsa.RSAEncrypt(msg);
  std::string encrypted_msg = my_rsa.RSAgetcryptotext();
  //send message
  int fail = write( client, encrypted_msg.c_str(), encrypted_msg.length()); 
  if ( fail < msg.length() ){
    perror( "write() failed" );
    return EXIT_FAILURE;
  }
  //get message
  if ((num_bytes = recv(client, buf, MSG_MAX-1, 0)) == -1) {
    perror("Error: recv failed");
    return EXIT_FAILURE;
  }
  buf[num_bytes] = '\0';
  // RSA Decrypt
  std::string received_msg(buf,sizeof(buf));
  my_rsa.RSADecrypt(received_msg);
  received_msg = my_rsa.RSAgetmessage();
  if (msg.compare(received_msg) != 0){
    perror( "Hacker detected" );
    return EXIT_FAILURE;
  }
  
  int broken = 0;
  for(int x = 0; x < 3; x++){
    mpz_t shared_key;
    mpz_init(shared_key);
    char hold[MSG_MAX];
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
      return EXIT_FAILURE;
    }
    if ((num_bytes = recv(client, hold, MSG_MAX-1, 0)) == -1) {
      perror("Error: recv failed");
      return EXIT_FAILURE;
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
        return EXIT_FAILURE;
      }
    }else if(x == 2){
      broken = aes::fill_iv(all_keys.aes_iv, shared_key);
      if(broken != 0){
        perror("Error: fill_key failed");
        return EXIT_FAILURE;
      }
    }
  }
  //send public keys
  //receive keys
  //make keys
  std::cout << "For depositing funds please use the format \"1 $amount\"" << std::endl;
  std::cout << "For withdrawing funds please use the format \"2 $amount\"" << std::endl;
  std::cout << "To deplay your balance please use the format \"3\"" << std::endl;
  std::cout << "To leave please uses \"q\"" << std::endl << std::endl;
  std::string message;
  while(1){
    char first[MSG_MAX];
    char last[MSG_MAX];
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
    if (message.length() > MSG_MAX){
        std::cout << "Message Aborted: Message was too long" << std::endl;
        continue;
    }
    // checks for the initial character
    // 1 = deposit (ssh)
    // 2 = withdraw (ssh)
    // 3 = balance (ssh)
    //ignore if message doesn't start with one of these 3
    else if (message[0] != ATMMsg::DEPOSIT && message[0] != ATMMsg::WITHDRAW && message[0] != ATMMsg::BALANCE){
        std::cout << "Message Aborted: Message had improper start" << std::endl;
        continue;
    }
    // checks for $
    else if (message[2] != '$'){
        std::cout << "Message Aborted: No $ detected" << std::endl;
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
    if ((num_bytes = recv(client, first, MSG_MAX-1, 0)) == -1) {
      perror("Error: recv failed");
      return EXIT_FAILURE;
    }
    first[num_bytes] = '\0';
    std::string en_msg(first,sizeof(first));
    if ((num_bytes = recv(client, last, MSG_MAX-1, 0)) == -1) {
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
    std::string gn_msg(holder2,sizeof(holder2));
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
