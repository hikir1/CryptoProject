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
    std::cout << "Client closed connection early." << std::endl;
    return -1;
  }
  std::cout << "recvd len: " << len << std::endl;
  return len;
}

int estab_con(int client, ssh::Keys all_keys, RSA &my_rsa){
  int num_bytes;
  char buf[ssh::RECV_MAX];
  std::string msg(ssh::HELLO_MSG,ssh::HELLO_LEN);
  std::string encrypted_msg;
  #ifndef NENCRYPT 
  #ifndef RSAENCRYPT
   encrypted_msg = my_rsa.RSAgetcryptotext(msg);
   #else
     encrypted_msg = received_msg;
    std::cout << encrypted_msg << std::endl;
  #endif
  #else
    encrypted_msg = msg;
  #endif
  //send message
  int fail = write( client, encrypted_msg.c_str(), ssh::HELLO_LEN); 
  if ( fail < msg.length() ){
    perror( "write() failed" );
    return -1;
  }
  //get message
  if ((num_bytes = recv(client, buf, ssh::HELLO_LEN, 0)) == -1) {
    perror("Error: recv failed");
    return -1;
  }
  buf[num_bytes] = '\0';
  // RSA Decrypt
  std::string received_msg(buf, ssh::HELLO_LEN);

  std::string decrypted_msg;
  #ifndef NENCRYPT
   #ifndef RSAENCRYPT
     decrypted_msg = my_rsa.RSAgetmessage(received_msg);
     #else
     decrypted_msg = received_msg;
    std::cout << decrypted_msg << std::endl;
   #endif
  #else
    decrypted_msg = received_msg;
    std::cout << decrypted_msg << std::endl;
  #endif
  if (msg.compare(decrypted_msg) != 0){

    fprintf(stderr, "Hacker detected\n" ); // <<<<<<<<<< changed from perror to fprintf(stderr,...)
    							//		(errno is not set; no error from libaray call)

    return -1;
  }
/*
  int broken = 0;
  for(int x = 0; x < 3; x++){ // <<<<<<<<< should only be sending 1 message with 3 keys
    std::string cryptotext;
 */
    char hold[ssh::KEYEX_LEN];
 /*
    #ifndef NENCRYPT
      mpz_t shared_key;
      mpz_init(shared_key);
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
      cryptotext = ctxt.get_str();
      fail = write( client, cryptotext.c_str(), ssh::KEYEX_LEN); 
      if ( fail < cryptotext.length() ){
        perror( "write() failed" );
        return -1;
      }
    #else
  */
  ssh::DiffieKeys diffieKeys;
   char chook[ssh::KEYEX_LEN] = {0};
  if (diffieKeys.genKeys(chook, all_keys) == -1) {
    std::cerr << "ERROR: failed to parse diffie keys" << std::endl;
    return -1;
  }

std::cerr << "here1" <<std::endl;

  // TODO: RSA Encrypt server key parts
  if (send(client, diffieKeys.pubKeys(), ssh::KEYEX_LEN, 0) == -1) {
    perror("ERROR: Failed to send keys.");
    return -1;
  }

std::cerr << "here2" << std::endl;
/*    #endif	*/

    //send keyhalf here
    if ((num_bytes = recv(client, hold, ssh::KEYEX_LEN, 0)) == -1) {
      perror("Error: recv failed");
      return -1;
    }

    diffieKeys.genKeys(hold, all_keys);
  return client;
}

namespace act { // <<<<<<<<<<<<<<<<<<< added this for convenience and clarity
  constexpr char DEPOSIT = '1';
  constexpr char WITHDRAW = '2';
  constexpr char BALANCE = '3';
}

// <<<<<<<<<<<<<<<<<<< need to put this everywhere (might want to split main()
		   			//				into smaller functions)
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
    int client = make_client(argv[1], argv[2]);
    if (client == -1)
      return EXIT_FAILURE;
    int test = estab_con(client, all_keys, my_rsa);
    if(test == -1){
      CLOSE_CLIENT
      return EXIT_FAILURE;
    }
    std::cout << "What user are you (0, 255):" << std::endl;

    if (!std::getline(std::cin, id)) { // <<<<<<< **** BAD: std::getline() does not return NULL on error
    								// TODO: check for exception!!!
      perror("Error: getline failed"); // <<< std::getline is a c++ function. errno not set. dont use perror()
	CLOSE_CLIENT
      return EXIT_FAILURE;
    }

    int i_d;
    try {	// <<<<<<<<<<<<<<<<<<<<<<<<<< added this try block

      i_d = stoi(id);
      if(i_d < 0 || i_d > 255){
        std::cerr << "Unsupported ID" << std::endl;
	  CLOSE_CLIENT
        continue;
	}

    }catch(const std::invalid_argument& ia){ // <<<<<<<<<<<<<<<< and these catches (copied from below)
       std::cerr << "Invalid id" << std::endl;
       std::cerr << "Message Aborted: Cannot be converted" << std::endl;
	 CLOSE_CLIENT
       continue;
    }catch(const std::out_of_range& oor){
       std::cerr << "Invalid id" << std::endl;
       std::cerr << "Message Aborted: Amount of funds too large" << std::endl;
	 CLOSE_CLIENT
       continue;
    }

    u_id = (unsigned char) i_d;

    std::cout << "For depositing funds please use the format \"" << act::DEPOSIT << " $amount\"" << std::endl;
    std::cout << "For withdrawing funds please use the format \"" << act::WITHDRAW << " $amount\"" << std::endl;
    std::cout << "To deplay your balance please use the format \"" << act::BALANCE << "\"" << std::endl;
    std::cout << "To leave please uses \"q\"" << std::endl << std::endl;
    std::cout << "Enter your transaction below:" << std::endl;

    if (!std::getline(std::cin, message)) { // <<<<<<<<<<< **** getline() doesnt return NULL
      perror("Error: getline failed");
	CLOSE_CLIENT
      return EXIT_FAILURE;
    }
    if(message.length() == 0){
      std::cout << "Message Aborted: Message was too short" << std::endl;
	CLOSE_CLIENT
      continue;
    }
    else if(message[0] == 'q'){
      std::cout << "Bye" << std::endl;
	CLOSE_CLIENT
      break;
    }

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< no need to test this (`message` is not being sent)
    // if length (ssh) msg_max is exceeded abort
    if (message.length() > ssh::RECV_MAX){
        std::cout << "Message Aborted: Message was too long" << std::endl;
        continue;
    }
*/

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< redundant: test for this later, in switch below
    // checks for the initial character
    // 1 = deposit (ssh)
    // 2 = withdraw (ssh)
    // 3 = balance (ssh)
    //ignore if message doesn't start with one of these 3
    else if (message[0] != (int)ssh::MsgType::DEPOSIT && message[0] != (int)ssh::MsgType::WITHDRAW && message[0] != (int)ssh::MsgType::BALANCE){
        std::cout << "Message Aborted: Message had improper start" << std::endl;
        continue;
    }
*/

/* <<<<<<<<<<<<<<<<<<<<<< not all message require $
    // checks for $
    else if (message[2] != '$'){
        std::cout << "Message Aborted: No $ detected" << std::endl;
        continue;
    }
*/

/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< use message.substr()
    std::string checker = "";
    for(int x = 3; x < message.length(); x++){
      checker = checker+message[x];
    }
*/

    unsigned long long int money = 0;

    if (message[0] == act::DEPOSIT || message[0] == act::WITHDRAW) { // <<<<<<<<<<<<<<<<<<<<<<<< added this

	if (message.size() < 4) { // <<<<<<<<<<<<< check for right size
        std::cout << "Message Aborted: No amount detected" << std::endl;
	  CLOSE_CLIENT
	  continue;
	}

      if (message[2] != '$') { // <<<<<<<<<<<<< Pulled from above
        std::cout << "Message Aborted: No $ detected" << std::endl;
	  CLOSE_CLIENT
        continue;
      }

      try{
        money = stoull(message.substr(3));

	  if (sizeof(uint64_t) < sizeof(unsigned long long) // <<<<<<< uint64_t and ull nott technically the same
	      && money > sizeof(uint64_t))
	    throw std::out_of_range("Exceeded maximum of uint64");

      }catch(const std::invalid_argument& ia){
         std::cerr << "Invalid amount" << std::endl;
         std::cerr << "Message Aborted: Cannot be converted" << std::endl;
	   CLOSE_CLIENT
         continue;
      }catch(const std::out_of_range& oor){
         std::cerr << "Invalid amount" << std::endl;
         std::cerr << "Message Aborted: Amount of funds too large" << std::endl;
	   CLOSE_CLIENT
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
        CLOSE_CLIENT
        continue;
/*	
        #ifdef NDEBUG
        std::cout << "Invalid message format" << std::endl;
        #else
        std::cout << "Unknown message class" << std::endl;
        #endif
*/
        // msgType = ssh::MsgType::BAD_FORMAT; // <<<<<<<<<<<<<<<< no need for this. just dont send message.
      }
    }
    if (send(client, ssh::SendMsg(msgType, u_id, money, all_keys) , ssh::TOTAL_LEN, 0) == -1) {
      perror("ERROR: Failed to send message");
	close(client);
      return -1;
    }

    char recvbuf[ssh::TOTAL_LEN];
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

    CLOSE_CLIENT
  }

  // sleep(5); // <<<<<<<<<<<< remove this
  return EXIT_SUCCESS;
}
