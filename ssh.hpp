#ifndef _SSH_HPP_
#define _SSH_HPP_
#include <cstring>
#include "aes/aes.hpp"
#include "hmac/hmac.h"
#include "KeyGen.hpp"

constexpr size_t RECV_MAX = 1024;

// after sym keys established
constexpr size_t MSG_LEN = sizeof(char[2]) + sizeof(uint64_t); // bytes
// include padding for aes
constexpr size_t AES_BUF_LEN = MSG_LEN + aes::BLOCK_BYTES - MSG_LEN % aes::BLOCK_BYTES; 
// include preceding hmac
constexpr size_t TOTAL_LEN = hmac::output_length + AES_BUF_LEN;

constexpr const char * HELLO_MSG = "I AM NOT A HACKER";
constexpr size_t HELLO_LEN = strlen(HELLO_MSG);

constexpr size_t KEYEX_LEN = 3 * KeyGen::diffiekeyhalfsize;

static_assert(HELLO_LEN <= RECV_MAX);
static_assert(KEYEX_LEN <= RECV_MAX);
static_assert(TOTAL_LEN <= RECV_MAX);

struct Keys {
	std::string hmac_key;
	aes::Key aes_key;
	aes::IV aes_iv;
};

namespace MsgType {
	enum Type {
		DEPOSIT,
		WITHDRAW,
		BALANCE,
		OK,
		BAD_FORMAT,
		TOO_MUCH_BANK,
		NOT_ENOUGH_DOUGH,
		INVALID
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

#endif
