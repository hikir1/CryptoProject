#ifndef _SSH_HPP_
#define _SSH_HPP_
#include <cstring>
#include "aes/aes.hpp"
#include "hmac/hmac.h"
#include "KeyGen.hpp"
#include <cmath>
#include <gmpxx.h>

namespace ssh {

constexpr size_t RECV_MAX = 1024;

// after sym keys established
constexpr size_t MSG_LEN = sizeof(char[2]) + sizeof(uint64_t); // bytes
// include padding for aes
constexpr size_t AES_BUF_LEN = MSG_LEN + aes::BLOCK_BYTES - MSG_LEN % aes::BLOCK_BYTES; 
// include preceding hmac
constexpr size_t TOTAL_LEN = hmac::output_length + AES_BUF_LEN;

constexpr const char * HELLO_MSG = "I AM NOT A HACKER";
constexpr size_t HELLO_LEN = strlen(HELLO_MSG) + 1; // + 1 for null byte

constexpr size_t CLIENT_KEYEX_LEN = 
		#ifdef NENCRYPT
			1;
		#else
			6 * KeyGen::diffiekeyhalfsize;
		#endif

constexpr size_t SERVER_KEYEX_LEN = 
		#ifdef NENCRYPT
			1;
		#else
			2 * KeyGen::diffiekeyhalfsize;
		#endif

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
	char buf[CLIENT_KEYEX_LEN];
	public:
	ClientDiffieKeys() : hmac_keys(KeyGen::createKeyhalf_client()),
			aes_keys(KeyGen::createKeyhalf_client()) {
		assert(hmac_keys[0].size() == KeyGen::diffiekeyhalfsize);
		assert(aes_keys[0].size() == KeyGen::diffiekeyhalfsize);
		for (int i = 0; i < 3; i++)
			memcpy(buf + i * KeyGen::diffiekeyhalfsize, hmac_keys[i].data(), KeyGen::diffiekeyhalfsize);
		for (int i = 0; i < 3; i++)
			memcpy(buf + (i+3) * KeyGen::diffiekeyhalfsize, aes_keys[i].data(), KeyGen::diffiekeyhalfsize);
	}
	const char * pubKeys() const {
		return buf;
	}
	int genKeys(const char keyex_msg[SERVER_KEYEX_LEN], Keys &keys);
};

class ServerDiffieKeys {	
	std::vector<std::string> hmac_keys;
	std::vector<std::string> aes_keys;
	std::string hmac_shared;
	std::string aes_shared;
	char buf[SERVER_KEYEX_LEN];
	public:
	ServerDiffieKeys(const char keyex_recv[CLIENT_KEYEX_LEN]) {
		std::vector<std::string> temp_hmac, temp_aes;
		for (int i = 0; i < 3; i++)
			temp_hmac.push_back(std::string(keyex_recv + i * KeyGen::diffiekeyhalfsize,
					KeyGen::diffiekeyhalfsize));
		for (int i = 0; i < 3; i++)
			temp_aes.push_back(std::string(keyex_recv + (i+3) * KeyGen::diffiekeyhalfsize,
					KeyGen::diffiekeyhalfsize));
		hmac_keys = KeyGen::createKeyhalf_server(temp_hmac[1], temp_hmac[2]);
		aes_keys = KeyGen::createKeyhalf_server(temp_aes[1], temp_aes[2]);
		assert(hmac_keys[0].size() == KeyGen::diffiekeyhalfsize);
		assert(aes_keys[0].size() == KeyGen::diffiekeyhalfsize);
		memcpy(buf, hmac_keys[0].data(), KeyGen::diffiekeyhalfsize);
		memcpy(buf + KeyGen::diffiekeyhalfsize, aes_keys[0].data(), KeyGen::diffiekeyhalfsize);
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

char* RSAGetCipherText(RSA myRSA, std::string message);

}

#endif
