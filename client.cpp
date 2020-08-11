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
#include "aes/aes.hpp"
#include "hmac/hmac.h"
#include "ssh.hpp"
#include "KeyEncryption.cpp"
#include "KeyGeneration.cpp"


int main(int argc, char ** argv)
{
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <HOST> <PORT>" << std::endl;
    return EXIT_FAILURE;
  }
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
    perror("Error: client failed to connect\n");
    return EXIT_FAILURE;
  }
  if ((num_bytes = recv(client, buf, MSG_MAX-1, 0)) == -1) {
    perror("Error: recv failed");
    return EXIT_FAILURE;
  }
  // TODO: RSA Encrypt
  //std::string answer = 
  int fail = write( client, string.c_str(), .length()); 
  if ( fail < strlen( msg ) ){
    perror( "write() failed" );
    return EXIT_FAILURE;
  }
  //read
  // TODO: RSA Decrypt


  buf[num_bytes] = '\0';

  printf("client: received '%s'\n",buf);
  std::cout << "For depositing funds please use the format \"1 $amount\"" << std::endl;
  std::cout << "For withdrawing funds please use the format \"2 $amount\"" << std::endl;
  std::cout << "To deplay your balance please use the format \"3\"" << std::endl << std::endl;;
  std::string message;
  while(1){
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

  }
  sleep(5);
  close(client);
  return EXIT_SUCCESS;
}
