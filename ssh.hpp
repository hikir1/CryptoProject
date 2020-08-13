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

constexpr size_t KEYEX_LEN = 
		#ifdef NENCRYPT
			1;
		#else
			2 * KeyGen::diffiekeyhalfsize;
		#endif

static_assert(HELLO_LEN <= RECV_MAX);
static_assert(KEYEX_LEN <= RECV_MAX);
static_assert(TOTAL_LEN <= RECV_MAX);

struct Keys {
	char hmac_key[hmac::byte_length];
	aes::Key aes_key;
	aes::IV aes_iv;
};

class DiffieKeys {	
	std::vector<std::string> hmac_keys;
	std::vector<std::string> aes_keys;
	char buf[KEYEX_LEN];
	public:
	DiffieKeys() : hmac_keys(KeyGen::createKeyhalf()),
			aes_keys(KeyGen::createKeyhalf()) {
		assert(hmac_keys[0].size() == KeyGen::diffiekeyhalfsize);
		assert(aes_keys[0].size() == KeyGen::diffiekeyhalfsize);
		memcpy(buf, hmac_keys[0].data(), KeyGen::diffiekeyhalfsize);
		memcpy(buf + KeyGen::diffiekeyhalfsize, aes_keys[0].data(), KeyGen::diffiekeyhalfsize);
	}
	const char * pubKeys() const {
		return buf;
	}
	int genKeys(const char keyex_msg[KEYEX_LEN], Keys &keys);
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

}

#endif
