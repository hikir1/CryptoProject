#ifndef _SSH_HPP_
#define _SSH_HPP_
#include <cstring>
#include "aes/aes.hpp"
#include "hmac/hmac.h"
#include "KeyGen.hpp"
#include "RSA/RSA.h"
#include <cmath>
#include <gmpxx.h>

namespace ssh {

constexpr size_t RECV_MAX = 1024;
constexpr size_t RSA_MAX = 256;
// after sym keys established
constexpr size_t MSG_LEN = sizeof(char[2]) + sizeof(uint64_t); // bytes
// include padding for aes
constexpr size_t AES_BUF_LEN = MSG_LEN + aes::BLOCK_BYTES - MSG_LEN % aes::BLOCK_BYTES; 
// include preceding hmac
constexpr size_t TOTAL_LEN = hmac::output_length + AES_BUF_LEN;

constexpr const char * HELLO_MSG = "I AM NOT A HACKER";
constexpr size_t HELLO_LEN = strlen(HELLO_MSG) + 1; // + 1 for null byte

constexpr size_t CLIENT_PTXT_KEYEX_LEN = 6 * KeyGen::diffiekeyhalfsize;
constexpr size_t SERVER_PTXT_KEYEX_LEN = 2 * KeyGen::diffiekeyhalfsize;

constexpr size_t CLIENT_KEYEX_LEN = CLIENT_PTXT_KEYEX_LEN
		+ (CLIENT_PTXT_KEYEX_LEN % RSA_MAX > 0? RSA_MAX - CLIENT_PTXT_KEYEX_LEN % RSA_MAX: 0);
constexpr size_t SERVER_KEYEX_LEN = SERVER_PTXT_KEYEX_LEN
		+ (SERVER_PTXT_KEYEX_LEN % RSA_MAX >0? RSA_MAX - CLIENT_PTXT_KEYEX_LEN % RSA_MAX: 0);

static_assert(HELLO_LEN <= RECV_MAX);
static_assert(CLIENT_KEYEX_LEN <= RECV_MAX);
static_assert(SERVER_KEYEX_LEN <= RECV_MAX);
static_assert(TOTAL_LEN <= RECV_MAX);

struct Keys {
	char hmac_key[hmac::byte_length];
	aes::Key aes_key;
	aes::IV aes_iv;
};

int genKeys(std::string hmac_shared, std::string aes_shared, ssh::Keys &keys);

class ClientDiffieKeys {	
	std::vector<std::string> hmac_keys;
	std::vector<std::string> aes_keys;
	char buf[CLIENT_KEYEX_LEN] = {0};
	public:
	ClientDiffieKeys(RSA &rsa) : hmac_keys(KeyGen::createKeyhalf_client()),
			aes_keys(KeyGen::createKeyhalf_client()) {
		assert(hmac_keys[0].size() == KeyGen::diffiekeyhalfsize);
		assert(aes_keys[0].size() == KeyGen::diffiekeyhalfsize);
		std::string ptxt1 = hmac_keys[0] + hmac_keys[1];
		std::string ptxt2 = hmac_keys[2] + aes_keys[0];
		std::string ptxt3 = aes_keys[1] + aes_keys[2];
		std::string ctxt = rsa.RSAgetcryptotext(ptxt1);
		ctxt += rsa.RSAgetcryptotext(ptxt2);
		ctxt += rsa.RSAgetcryptotext(ptxt3);
		assert(ctxt.size() == CLIENT_KEYEX_LEN);
		memcpy(buf, ctxt.data(), CLIENT_KEYEX_LEN);
	}
	const char * pubKeys() const {
		return buf;
	}
	int genKeys(const char keyex_msg[SERVER_KEYEX_LEN], RSA &rsa, Keys &keys);
};

class ServerDiffieKeys {	
	std::vector<std::string> hmac_keys;
	std::vector<std::string> aes_keys;
	std::string hmac_shared;
	std::string aes_shared;
	char buf[SERVER_KEYEX_LEN] = {0};
	public:
	ServerDiffieKeys(const char keyex_recv[CLIENT_KEYEX_LEN], RSA &rsa) {
		std::string ctxt1 = std::string(keyex_recv, RSA_MAX);
		std::string ctxt2 = std::string(keyex_recv + RSA_MAX, RSA_MAX);
		std::string ctxt3 = std::string(keyex_recv + 2*RSA_MAX, RSA_MAX);
		std::string ptxt1 = rsa.RSAgetmessage(ctxt1);
		std::string ptxt2 = rsa.RSAgetmessage(ctxt2);
		std::string ptxt3 = rsa.RSAgetmessage(ctxt3);
		std::vector<std::string> temp_hmac, temp_aes;
		temp_hmac.push_back(ptxt1.substr(0, KeyGen::diffiekeyhalfsize));
		temp_hmac.push_back(ptxt1.substr(KeyGen::diffiekeyhalfsize));
		temp_hmac.push_back(ptxt2.substr(0, KeyGen::diffiekeyhalfsize));
		temp_aes.push_back(ptxt2.substr(KeyGen::diffiekeyhalfsize));
		temp_aes.push_back(ptxt3.substr(0, KeyGen::diffiekeyhalfsize));
		temp_aes.push_back(ptxt3.substr(KeyGen::diffiekeyhalfsize));
		
		hmac_keys = KeyGen::createKeyhalf_server(temp_hmac[1], temp_hmac[2]);
		aes_keys = KeyGen::createKeyhalf_server(temp_aes[1], temp_aes[2]);
		assert(hmac_keys[0].size() == KeyGen::diffiekeyhalfsize);
		assert(aes_keys[0].size() == KeyGen::diffiekeyhalfsize);

		std::string send_ctxt = rsa.RSAgetcryptotext(hmac_keys[0] + aes_keys[0]);
		assert(send_ctxt.size() == SERVER_KEYEX_LEN);
		memcpy(buf, send_ctxt.data(), send_ctxt.size());

		hmac_shared = KeyGen::getSharedKey(hmac_keys, temp_hmac[0]);
		aes_shared = KeyGen::getSharedKey(aes_keys, temp_aes[0]);
	}
	const char * pubKeys() const {
		return buf;
	}
	int genKeys(Keys &keys);
};

namespace MsgType {
	enum Type {
		DEPOSIT, //0
		WITHDRAW, //1
		BALANCE, //2
		OK, //3
		BAD_FORMAT, //4
		TOO_MUCH_BANK, //5
		NOT_ENOUGH_DOUGH, //6
		INVALID //7
	};
}

struct RecvMsg {
	MsgType::Type type = MsgType::INVALID;
	unsigned char uid = 0;
	uint64_t amt = 0;
	const char * error = nullptr;
	RecvMsg(const char msg[TOTAL_LEN], size_t recvlen, const Keys &keys);
};

struct SendMsg {
	private:
	char msg[TOTAL_LEN] = {0};
	public:
	SendMsg(MsgType::Type type, unsigned char uid, uint64_t amt, const Keys &keys);
	operator const char*() const {
		return msg;
	}
};

std::string RSAGetCipherText(RSA myRSA, std::string message);
std::string RSAGetPlainText(RSA myRSA, std::string ciphertext);
}

#endif
