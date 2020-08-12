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

ssize_t try_recv(int cd, char * buf, size_t buflen) {
  assert(buflen < INT_MAX);
  ssize_t len = recv(cd, buf, buflen - 1, MSG_WAITALL);
  if (len == -1) {
    perror("ERROR: Failed to recv message");
    return -1;
  }
  if (len == 0) {
    std::cout << "Client closed connection early." << std::endl;
    return -1;
  }
  std::cout << "recvd len: " << len << std::endl;
  buf[len] = '\0';
  return len;
}

int estab_con(int client, ssh::Keys all_keys, RSA my_rsa){
  int num_bytes;
  char buf[ssh::RECV_MAX];
  std::string msg(ssh::HELLO_MSG,ssh::HELLO_LEN);
  std::string encrypted_msg;
  #ifndef NENCRYPT
   encrypted_msg = my_rsa.RSAgetcryptotext(msg);
  #else
    encrypted_msg = msg;
  #endif
  //send message
  int fail = write( client, ssh::HELLO_MSG, ssh::HELLO_LEN); 
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
  std::string received_msg(buf,strlen(buf));
  std::string decrypted_msg;
  #ifndef NENCRYPT
   decrypted_msg = my_rsa.RSAgetmessage(received_msg);
  #else
    decrypted_msg = received_msg;
    std::cout << decrypted_msg << std::endl;
  #endif
  if (msg.compare(decrypted_msg) != 0){
    perror( "Hacker detected" );
    return -1;
  }
  int broken = 0;
  for(int x = 0; x < 3; x++){
    std::string cryptotext;
    char hold[ssh::RECV_MAX];
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
    #else
      cryptotext = "Check";
    #endif
    //send keyhalf here
    fail = write( client, cryptotext.c_str(), ssh::KEYEX_LEN); 
    if ( fail < cryptotext.length() ){
      perror( "write() failed" );
      return -1;
    }
    if ((num_bytes = recv(client, hold, ssh::KEYEX_LEN, 0)) == -1) {
      perror("Error: recv failed");
      return -1;
    }
    hold[num_bytes] = '\0';
    //receive key as char*
    #ifndef NENCRYPT
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
    #else
      std::cout << hold << std::endl;
    #endif
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
  #ifndef NENCRYPT
    my_rsa.LoadKeys("clientKeys");
  #endif
  //send public keys
  //receive keys
  //make keys
  std::string message;
  std::string id;
  unsigned char u_id;
  while(1){
    int test = estab_con(client, all_keys, my_rsa);
    if(test == -1){
      return EXIT_FAILURE;
    }
    std::cout << "What user are you (0, 255):" << std::endl;
    if (!std::getline(std::cin, id)) {
      perror("Error: getline failed");
      return EXIT_FAILURE;
    }
    int i_d = stoi(id);
    if(i_d < 0 || i_d > 255){
      std::cerr << "Unsupported ID" << std::endl;
      continue;
    }
    u_id = (unsigned char) i_d;
    std::cout << "For depositing funds please use the format \"1 $amount\"" << std::endl;
    std::cout << "For withdrawing funds please use the format \"2 $amount\"" << std::endl;
    std::cout << "To deplay your balance please use the format \"$\"" << std::endl;
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
    unsigned long long int money = 0;
    for(int x = 3; x < message.length(); x++){
      checker = checker+message[x];
    }
    try{
      money = stoull(checker);
    }catch(const std::invalid_argument& ia){
       std::cerr << "Invalid arguments: " << ia.what() << std::endl;
       std::cerr << "Message Aborted: Cannot be converted" << std::endl;
       continue;
    }catch(const std::out_of_range& oor){
       std::cerr << "Invalid arguments: " << oor.what() << std::endl;
       std::cerr << "Message Aborted: Amount of funds too large" << std::endl;
       continue;
    }

    ssh::MsgType::Type msgType;
    switch(message[0]){
      case (int)ssh::MsgType::DEPOSIT: {
        msgType = ssh::MsgType::DEPOSIT;
      } break;
      case (int)ssh::MsgType::WITHDRAW: {
        msgType = ssh::MsgType::WITHDRAW;
      } break;
      case (int)ssh::MsgType::BALANCE: {
        msgType = ssh::MsgType::BALANCE;
      } break;
      default: {
        #ifdef NDEBUG
        std::cout << "Invalid message format" << std::endl;
        #else
        std::cout << "Unknown message class" << std::endl;
        #endif
        msgType = ssh::MsgType::BAD_FORMAT;
      }
    }
    if (send(client, ssh::SendMsg(msgType, u_id, money, all_keys) , ssh::TOTAL_LEN, 0) == -1) {
      perror("ERROR: Failed to send message");
      return -1;
    }
    first[num_bytes] = '\0';
    char recvbuf[ssh::TOTAL_LEN];
    ssize_t recvlen;
    if ((recvlen = try_recv(client, recvbuf, ssh::TOTAL_LEN)) == -1){
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
            std::cout << "A deposit of " << checker << " was accepted, the balance is now " << msg.amt << std::endl;
          } break;
          case ssh::MsgType::WITHDRAW: {
            std::cout << "A withdrawal of " << checker << " was accepted, the balance is now " << msg.amt << std::endl;
          } break;
          case ssh::MsgType::BALANCE: {
            std::cout << "The balance is " << msg.amt << std::endl;
          } break;
        }
      }break;
      case(ssh::MsgType::BAD_FORMAT):{
        switch(msgType){
          case ssh::MsgType::DEPOSIT: {
            std::cout << "A deposit of " << checker << " was denied due to a bad format" << std::endl;
          } break;
          case ssh::MsgType::WITHDRAW: {
            std::cout << "A withdrawal of " << checker << " was denied due to a bad format" << std::endl;
          } break;
          case ssh::MsgType::BALANCE: {
            std::cout << "A check of the balance was denied due to a bad format" << std::endl;
          } break;
        }
      }break;
      case(ssh::MsgType::TOO_MUCH_BANK):{
        std::cout << "A deposit of " << checker << " was denied due your bank account being too full" << std::endl;
      }break;
      case(ssh::MsgType::NOT_ENOUGH_DOUGH):{
        std::cout << "A withdrawal of " << checker << " was denied due your bank account being too empty" << std::endl;
      }break;
      case(ssh::MsgType::INVALID):{
        std::cout << "The message was invalid" << std::endl;
      }break;
    }
  }
  sleep(5);
  if (close(client) == -1) {
      perror("ERROR: Failed to close socket");
      return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
