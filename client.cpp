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
#include "RSA.cpp"
#include "KeyGeneration.cpp"


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
    perror("Error: getaddrinfo failed\n");
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
    perror("Error: client failed to connect\n");
    return EXIT_FAILURE;
  }

  // RSA Encrypt
  RSA my_rsa();
  std::string msg(HELLO_MSG,sizeof(HELLO_MSG));
  my_rsa.Encrypt(msg);
  std::string encrypted_msg = my_rsa.RSAgetcryptotext();
  //send message
  int fail = write( client, encrypted_msg.c_str(), encrypted_msg.length()); 
  if ( fail < strlen( msg ) ){
    perror( "write() failed\n" );
    return EXIT_FAILURE;
  }
  //get message
  if ((num_bytes = recv(client, buf, MSG_MAX-1, 0)) == -1) {
    perror("Error: recv failed\n");
    return EXIT_FAILURE;
  }
  buf[num_bytes] = '\0';
  // RSA Decrypt
  std::string received_msg(buf,sizeof(buf));
  my_rsa.Decrypt(received_msg);
  received_msg = my_rsa.RSAgetmessage();
  if (strcmp(msg, received_msg) != 0){
    perror( "Hacker detected\n" );
    return EXIT_FAILURE;
  }
  mpz_t shared_key;
  mpz_init(shared_key);
  for(int x = 0; x < 3; x++){
    char hold[MSG_MAX];
    mpz_t keyhalf;
    mpz_init(keyhalf);
    mpz_t p;
    mpz_init(p);
    mpz_t pkb;
    mpz_init(pkb);
    mpz_t other_keyhalf;
    mpz_init(other_keyhalf);
    KeyExchange(keyhalf, p, pkb); //keyhalf has proper values after this
    mpz_class ctxt(keyhalf);
    std::string cryptotext = ctxt.get_str();
    //send keyhalf here
    fail = write( client, cryptotext.c_str(), cryptotext.length()); 
    if ( fail < strlen( msg ) ){
      perror( "write() failed\n" );
      return EXIT_FAILURE;
    }
    if ((num_bytes = recv(client, hold, MSG_MAX-1, 0)) == -1) {
      perror("Error: recv failed\n");
      return EXIT_FAILURE;
    }
    hold[num_bytes] = '\0';
    //receive key as char*
    mpz_set_str(hold, other_key_half, 10) // this converts char* to key half
    sharedkey(shared_key, other_key, p, pkb) //stores shared key in shared_key after this
    if(x == 0){
      mpz_class var(shared_key);
      cryptotext = var.get_str();
      all_keys.hmac_key = cryptotext;
    }else if(x == 1){

    }else if(x == 2){

    }
  }
  //send public keys
  //receive keys
  //make keys
  std::cout << "For depositing funds please use the format \"1 $amount\"" << std::endl;
  std::cout << "For withdrawing funds please use the format \"2 $amount\"" << std::endl;
  std::cout << "To deplay your balance please use the format \"3\"" << std::endl << std::endl;;
  std::string message;
  while(1){
    char last[MSG_MAX];
    std::cout << "Enter your transaction below:" << std::endl;
    if (!std::getline(std::cin, message)) {
      perror("Error: getline failed");
      return EXIT_FAILURE;
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
    if (message[0] != BankMsg::DEPOSIT && message[0] != BankMsg::WITHDRAW && message[0] != BankMsg::BALANCE){
        std::cout << "Message Aborted: Message had improper start" << std::endl;
        continue;
    }
    // checks for $
    if (message[2] != '$'){
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
    if ( fail < strlen( msg ) ){
      perror( "write() failed\n" );
      return EXIT_FAILURE;
    }


    
    if ((num_bytes = recv(client, hold, MSG_MAX-1, 0)) == -1) {
      perror("Error: recv failed\n");
      return EXIT_FAILURE;
    }
    hold[num_bytes] = '\0';



  }
  sleep(5);
  if (close(client) == -1) {
      perror("ERROR: Failed to close socket");
      return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
