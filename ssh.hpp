#ifndef _SSH_HPP_
#define _SSH_HPP_
#include <cstring>
#include "aes/aes.hpp"

constexpr size_t MSG_MAX = 1024; // bytes

constexpr const char * HELLO_MSG = "I AM NOT A HACKER";
constexpr size_t HELLO_LEN = strlen(HELLO_MSG) + 1;
static_assert(HELLO_LEN <= MSG_MAX);

[[deprecated]] // define in diffie helman header
constexpr size_t PARKEY_LEN = 16;

constexpr size_t KEYEX_LEN = 3 * PARKEY_LEN;

struct Keys {
	std::string hmac_key;
	aes::Key aes_key;
	aes::IV aes_iv;
};

enum BankMsg {
	DEPOSIT = '1';
	WITHDRAW = '2';
	BALANCE = '3';
};

#endif
