#ifdef BANK

#include <iostream>
#include <cstdlib>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "ssh.hpp"
#include "aes/aes.hpp"
#include "hmac/hmac.h"
#include <cassert>
#ifndef NDEBUG
#include <climits>
#endif

extern "C" {
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netdb.h>
	#include <unistd.h>
	#include <signal.h>
}

int make_and_bind_sd(char * host, char * port) {
	// try to find HOST and PORT
	struct addrinfo hints = {0};
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
	struct addrinfo * res, * itr;
	int err = getaddrinfo(host, port, &hints, &res);
	if (err || !res) {
		fprintf(stderr, "ERROR: Can't use specified HOST/PORT: %s\n", gai_strerror(err));
		return -1;
	}
	itr = res;
	err = 0;
	int sd;
	while (itr) {
		if(!(sd = socket(res->ai_family, SOCK_STREAM, 0))) {
			perror("ERROR: Failed to build a socket");
			freeaddrinfo(res);
			return -1;
		}
		errno = 0;
		if (bind(sd, res->ai_addr, res->ai_addrlen) == 0) {
			freeaddrinfo(res);
			return sd;
		}
		err = errno;
		if(close(sd) == -1) {
			perror("ERROR: Failed to close unused socket");
			freeaddrinfo(res);
			return -1;
		}
		itr = itr->ai_next;
	}
	freeaddrinfo(res);
	fputs("ERROR: Can't use specified HOST/PORT", stderr);
	if (err) {
		fputs(": ", stderr);
		errno = err;
		perror(NULL);
	}
	else
		fputc('\n', stderr);
	return -1;
}

static bool gtimeout = false;
void timeout(int signum) { gtimeout = true; }

ssize_t try_recv(int cd, char * buf, size_t buflen) {
	assert(buflen < INT_MAX);
	constexpr unsigned int TIMEOUT = 3; // seconds
	gtimeout = false;
	#ifndef NENCRYPT
	alarm(TIMEOUT);
	#endif
	ssize_t len = recv(cd, buf, buflen - 1, MSG_WAITALL);
	alarm(0);
	if (gtimeout) {
		std::cout << "Client took too long to respond. Connection timed out." << std::endl;
		return -1;
	}
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

int hello(int cd) {
	char buf[HELLO_LEN] = {0};
	if (try_recv(cd, buf, HELLO_LEN) == -1)
		return -1;

	// TODO: RSA Decrypt

	if (strlen(buf) != strlen(HELLO_MSG) || strcmp(buf, HELLO_MSG) != 0) {
		std::cout << "Received invalid HELLO" << std::endl;
		return -1;
	}

	strcpy(buf, HELLO_MSG); // extra null byte at end (Only one bank, only one id)

	// TODO: RSA Encrypt

	if (send(cd, buf, HELLO_LEN /* TODO change this? */, 0) == -1) {
		std::cout << "Failed to send HELLO to client" << std::endl;
		return -1;
	}

	return 0;
}

int keyex(int cd, Keys &keys) {
	char buf[KEYEX_LEN + 1] = {0};
	if (try_recv(cd, buf, KEYEX_LEN + 1) == -1)
		return -1;
	std::cout << "recvd keyex" << std::endl;
	// TODO: RSA Decrypt

	// TODO: Parse client key parts
	// TODO: gen keys

	// TODO: RSA Encrypt server key parts
	// TODO: send server key parts
	
	return 0;
}

int bank(int cd, const Keys &keys) {
	char recvbuf[ssh::TOTAL_LEN];
	ssize_t recvlen;
	if ((recvlen = try_recv(cd, recvbuf, ssh::TOTAL_LEN)) == -1)
		return -1;
	ssh::RecvMsg msg(recvbuf, recvlen, keys);
	if (msg.error) {
		std::cout << msg.error << std::endl;
		msg.type = ssh::MsgType::INVALID;
	}

	static uint64_t safe[UCHAR_MAX] = {0};

	ssh::MsgType::Type msgType = ssh::MsgType::OK;
	uint64_t msgAmt = 0;

	switch (msg.type) {
	case ssh::MsgType::DEPOSIT: {
		if (msg.amt > UINT64_MAX - safe[msg.uid]) {
			std::cout << "Client attempted to deposit more than maximum." << std::endl;
			msgType = ssh::MsgType::TOO_MUCH_BANK;
			break;
		}
		msgAmt = safe[uid] += dep;
		#ifndef NDEBUG
		std::cout << "New Balance for " << (unsigned int) uid << " is " << safe[uid] << std::endl;
		#endif
	} break;
	case ssh::MsgType::WITHDRAW: {
		if (msg.amt > safe[msg.uid]) {
			std::cout << "Client attempted to withdraw more than they had." << std::endl;
			msgType = BankMsg::NOT_ENOUGH_DOUGH;
			ctxt[1] = 0;
			break;
		}
		msgAmt = safe[uid] -= wd;
		#ifndef NDEBUG
		std::cout << "New Balance for " << (unsigned int) uid << " is " << safe[uid] << std::endl;
		#endif
		ctxt[0] = BankMsg::OK;
		ctxt[1] = 0;
	} break;
	case ssh::MsgType::BALANCE: {
		msgAmt = safe[uid];
		#ifndef NDEBUG
		std::cout << "Current balance for " << (unsigned int) uid << " is " << bal << std::endl;
		#endif
	} break;
	default: {
		#ifdef NDEBUG
		std::cout << "Invalid message format" << std::endl;
		#else
		std::cout << "Unknown message class " << ptxt[0] << std::endl;
		#endif
		msgType = ssh::MsgType::BAD_FORMAT;
	}
	}

	if (send(cd, ssh::SendMsg(msgType, msg.uid, msgAmt) , newlen, 0) == -1) {
		perror("ERROR: Failed to send message");
		return -1;
	}
	
	return 0;
}

int serve(int cd) {
	Keys keys;
	if (hello(cd) == -1 
			|| keyex(cd, keys) == -1 
			|| bank(cd, keys) == -1)
		return -1;
	return 0;
}

inline int imtired(int cd) {
	return serve(cd);
}

int main(int argc, char ** argv) {
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <HOST> <PORT>" << std::endl;
		return EXIT_FAILURE;
	}

	int sd = make_and_bind_sd(argv[1], argv[2]);
	if (sd == -1)
		return EXIT_FAILURE;

	constexpr int backlog = 128;
	if (listen(sd, backlog) == -1) {
		perror("ERROR: Socket failed to listen");
		close(sd);
		return EXIT_FAILURE;
	}

	struct sigaction sigact = {0};
	sigact.sa_handler = timeout;
	if (sigaction(SIGALRM, &sigact, NULL) == -1) {
		perror("ERROR: Failed to set timeout alarm handler");
		close(sd);
		return EXIT_FAILURE;
	}

	int cd; // client descriptor
	while (true) {
		cd = accept(sd, nullptr, nullptr);
		if (cd == -1) {
			perror("Failed to accept connection");
			close(sd);
			return EXIT_FAILURE;
		}
		imtired(cd);
		if (shutdown(cd, SHUT_RDWR) == -1) {
			perror("ERROR: Failed to shutdown server side of connection");
			close(cd);
			close(sd);
			return EXIT_FAILURE;
		}
		if (close(cd) == -1) {
			perror("ERROR: Failed to close client socket");
			close(sd);
			return EXIT_FAILURE;
		}
	}
	

}

#endif
