#ifndef _SSH_HPP_
#define _SSH_HPP_

constexpr size_t MSG_MAX = 1024; // bytes

constexpr const char * HELLO_MSG = "I AM NOT A HACKER";
constexpr size_t HELLO_LEN = strlen(HELLO_MSG) + 1;
static_assert(HELLO_LEN <= MSG_MAX);

enum BankMsg {
	DEPOSIT = 1;
	WITHDRAW = 2;
	BALANCE = 3;
};

#endif
