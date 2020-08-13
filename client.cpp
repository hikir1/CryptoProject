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
  int client;
  int value;
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
	  itr = itr->ai_next;
        continue;
    }
    if (connect(client, res->ai_addr, res->ai_addrlen) == -1) {
        close(client);
	  itr = itr->ai_next;
        continue;
    }
    break;
  }
  freeaddrinfo(res); // all done with this structure
  if (itr == NULL) {
    perror("Error: client failed to connect");
    return -1;
  }
  return client;
}

ssize_t try_recv(int cd, char * buf, size_t buflen) {
  assert(buflen < INT_MAX);
  ssize_t len = recv(cd, buf, buflen, MSG_WAITALL);
  if (len == -1) {
    perror("ERROR: Failed to recv message");
    return -1;
  }
  if (len == 0) {
    std::cout << "Server closed connection early." << std::endl;
    return -1;
  }
  std::cout << "recvd len: " << len << std::endl;
  return len;
}

int estab_con(int client, ssh::Keys &all_keys, RSA &my_rsa){
  int num_bytes;
  char buf[ssh::RSA_MAX] = {0};
  std::string msg(ssh::HELLO_MSG);
  std::string encrypted_msg;
  #if !(NENCRYPT || NRSA)
   encrypted_msg = ssh::RSAGetCipherText(my_rsa, msg);
   assert(encrypted_msg.size() == ssh::RSA_MAX);
  #else
    encrypted_msg = msg;
  #endif
  std::cerr << "Encrypted message: " << encrypted_msg << std::endl;
  //send message
  int fail = write( client, encrypted_msg.data(), ssh::RSA_MAX); 
  if ( fail < msg.length() ){
    perror( "write() failed" );
    return -1;
  }
  //get message
  if ((num_bytes = recv(client, buf, ssh::RSA_MAX, MSG_WAITALL)) == -1) {
    perror("Error: recv failed");
    return -1;
  }
  buf[num_bytes] = '\0';
  // RSA Decrypt
  std::string received_msg(buf, ssh::RSA_MAX);

  std::string decrypted_msg;
  #if !(NENCRYPT || NRSA)
     decrypted_msg = ssh::RSAGetPlainText(my_rsa, received_msg);
  #else
     decrypted_msg = received_msg;
    std::cout << decrypted_msg << std::endl;
  #endif
  if (msg.compare(decrypted_msg) != 0){
  std::cerr << "-----dec: " << decrypted_msg << std::endl;
    fprintf(stderr, "Hacker detected\n" );
    return -1;
  }

  char hold[ssh::SERVER_KEYEX_LEN] = {0};

  ssh::ClientDiffieKeys diffieKeys(my_rsa);
  // RSA Encrypt server key parts
  if (send(client, diffieKeys.pubKeys(), ssh::CLIENT_KEYEX_LEN, 0) == -1) {
    perror("ERROR: Failed to send keys.");
    return -1;
  }
  //send keyhalf here
  if ((num_bytes = recv(client, hold, ssh::SERVER_KEYEX_LEN, 0)) == -1) {
    perror("Error: recv failed");
    return -1;
  }
  if (diffieKeys.genKeys(hold, my_rsa, all_keys) == -1) {
    std::cerr << "ERROR: failed to parse diffie keys" << std::endl;
    return -1;
  }
  return client;
}

namespace act { // <<<<<<<<<<<<<<<<<<< added this for convenience and clarity
  constexpr char DEPOSIT = '1';
  constexpr char WITHDRAW = '2';
  constexpr char BALANCE = '3';
}

#define CLOSE_CLIENT \
    if (close(client) == -1) { \
      perror("ERROR: Failed to close socket"); \
      return EXIT_FAILURE; \
    }


int main(int argc, char ** argv)
{
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <HOST> <PORT>" << std::endl;
    return EXIT_FAILURE;
  }
  ssh::Keys all_keys;
  RSA my_rsa;
  // RSA Encrypt
  my_rsa.LoadKeys("clientKeys");
  //send public keys
  //receive keys
  //make keys
  std::string message;
  std::string id;
  unsigned char u_id;
  while(1){
    std::cout << "What user are you (0, 255):" << std::endl;
    if (!std::getline(std::cin, id)) {
      perror("Error: getline failed");
      return EXIT_FAILURE;
    }

    int i_d;
    try { // <<<<<<<<<<<<<<<<<<<<<<<<<< added this try block
      i_d = stoi(id);
      if(i_d < 0 || i_d > 255){
        std::cerr << "Unsupported ID" << std::endl;
        continue;
    }

    }catch(const std::invalid_argument& ia){ // <<<<<<<<<<<<<<<< and these catches (copied from below)
       std::cerr << "Invalid id" << std::endl;
       std::cerr << "Message Aborted: Cannot be converted" << std::endl;
       continue;
    }catch(const std::out_of_range& oor){
       std::cerr << "Invalid id" << std::endl;
       std::cerr << "Message Aborted: Amount of funds too large" << std::endl;
       continue;
    }

    u_id = (unsigned char) i_d;

    std::cout << "For depositing funds please use the format \"" << act::DEPOSIT << " $amount\"" << std::endl;
    std::cout << "For withdrawing funds please use the format \"" << act::WITHDRAW << " $amount\"" << std::endl;
    std::cout << "To display your balance please use the format \"" << act::BALANCE << "\"" << std::endl;
    std::cout << "To leave please uses \"q\"" << std::endl << std::endl;
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

    uint64_t money = 0;

    if (message[0] == act::DEPOSIT || message[0] == act::WITHDRAW) { // <<<<<<<<<<<<<<<<<<<<<<<< added this

    if (message.size() < 4) { // <<<<<<<<<<<<< check for right size
          std::cout << "Message Aborted: No amount detected" << std::endl;
         continue;
    }

      if (message[2] != '$') { // <<<<<<<<<<<<< Pulled from above
        std::cout << "Message Aborted: No $ detected" << std::endl;
        continue;
      }

      try{
        long try_money = stol(message.substr(3));
	  if (try_money < 0)
	  	throw std::out_of_range("stol");
	  money = (uint64_t) try_money;

      }catch(const std::invalid_argument& ia){
         std::cerr << "Invalid amount" << std::endl;
         std::cerr << "Message Aborted: Cannot be converted" << std::endl;
         continue;
      }catch(const std::out_of_range& oor){
         std::cerr << "Invalid amount" << std::endl;
         std::cerr << "Message Aborted: Amount of funds out of range" << std::endl;
         continue;
      }

    }

    ssh::MsgType::Type msgType;
    switch(message[0]){
      case act::DEPOSIT: {
        msgType = ssh::MsgType::DEPOSIT;
      } break;
      case act::WITHDRAW: {
        msgType = ssh::MsgType::WITHDRAW;
      } break;
      case act::BALANCE: {
        msgType = ssh::MsgType::BALANCE;
      } break;
      default: {
    
        std::cout << "Message Aborted: Message had improper start" << std::endl; // <<<<<<< moved from above
        continue;

      }
    }
    int client = make_client(argv[1], argv[2]);
    if (client == -1){
      return EXIT_FAILURE;
    }

    int test = estab_con(client, all_keys, my_rsa);
    if(test == -1){
      CLOSE_CLIENT
      return EXIT_FAILURE;
    }
    if (send(client, ssh::SendMsg(msgType, u_id, money, all_keys) , ssh::TOTAL_LEN, 0) == -1) {
      perror("ERROR: Failed to send message");
	    close(client);
      return -1;
    }
    char recvbuf[ssh::TOTAL_LEN] = {0};
    ssize_t recvlen;
    if ((recvlen = try_recv(client, recvbuf, ssh::TOTAL_LEN)) == -1){
      close(client);
      return -1;
    }
    ssh::RecvMsg msg(recvbuf, recvlen, all_keys);
    if (msg.error) {
      std::cout << msg.error << std::endl;
      msg.type = ssh::MsgType::INVALID;
    }
    switch(msg.type){
      case(ssh::MsgType::OK):{
        switch(msgType){
          case ssh::MsgType::DEPOSIT: {
            std::cout << "A deposit of " << money
		    << " was accepted, the balance is now " << msg.amt << std::endl;
          } break;
          case ssh::MsgType::WITHDRAW: {
            std::cout << "A withdrawal of " << money
		    << " was accepted, the balance is now " << msg.amt << std::endl;
          } break;
          case ssh::MsgType::BALANCE: {
            std::cout << "The balance is " << msg.amt << std::endl;
          } break;
        }
      }break;
      case(ssh::MsgType::BAD_FORMAT):{
        switch(msgType){
          case ssh::MsgType::DEPOSIT: {
            std::cout << "A deposit of " << money << " was denied due to a bad format" << std::endl;
          } break;
          case ssh::MsgType::WITHDRAW: {
            std::cout << "A withdrawal of " << money << " was denied due to a bad format" << std::endl;
          } break;
          case ssh::MsgType::BALANCE: {
            std::cout << "A check of the balance was denied due to a bad format" << std::endl;
          } break;
        }
      }break;
      case(ssh::MsgType::TOO_MUCH_BANK):{
        std::cout << "A deposit of " << money
	      << " was denied due your bank account being too full" << std::endl;
      }break;
      case(ssh::MsgType::NOT_ENOUGH_DOUGH):{
        std::cout << "A withdrawal of " << money
	      << " was denied due your bank account being too empty" << std::endl;
      }break;
      case(ssh::MsgType::INVALID):{
        std::cout << "The message was invalid" << std::endl;
      }break;
    }
    std::cout << std::endl << std::endl;

    CLOSE_CLIENT
  }
  return EXIT_SUCCESS;
}
