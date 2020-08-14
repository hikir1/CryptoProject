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
	ssize_t len = recv(cd, buf, buflen, MSG_WAITALL);
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
	if (len < buflen) {
		std::cerr << "Invalid message length." << std::endl;
		return -1;
	}
	#ifndef NDEBUG
	std::cout << "recvd len: " << len << std::endl;
	#endif
	return len;
}

int hello(int cd, RSA &rsa) {
	char buf[ssh::RSA_MAX] = {0};
	#ifndef NRSA
	if (try_recv(cd, buf, ssh::RSA_MAX) == -1)
		return -1;
	if (ssh::RSAGetPlainText(rsa, std::string(buf, ssh::RSA_MAX)).compare(ssh::HELLO_MSG) != 0) {
		std::cout << "Received invalid HELLO" << std::endl;
		std::cerr << "Hello: " << ssh::RSAGetPlainText(rsa, std::string(buf, ssh::RSA_MAX)) << "|" << std::endl;
		return -1;
	}
	std::cerr << "Hello: " << ssh::RSAGetPlainText(rsa, std::string(buf, ssh::RSA_MAX)) << "|" << std::endl;

	std::string ctxt = ssh::RSAGetCipherText(rsa, std::string(ssh::HELLO_MSG, ssh::HELLO_LEN));
	std::cerr << "'''''''''''' ctxt here: " << ctxt << std::endl;
	assert(ctxt.size() == ssh::RSA_MAX);
	if (send(cd, ctxt.data(), ssh::RSA_MAX, 0) != ssh::RSA_MAX) {
		std::cout << "Failed to send HELLO to client" << std::endl;
		return -1;
	}

	#else

	if (try_recv(cd, buf, ssh::HELLO_LEN) == -1)
		return -1;
	if (strncmp(buf, ssh::HELLO_MSG, ssh::HELLO_LEN) != 0) {
		std::cout << "Received invalid HELLO" << std::endl;
		return -1;
	}
	if (send(cd, ssh::HELLO_MSG, ssh::HELLO_LEN, 0) != ssh::HELLO_LEN) {
		std::cout << "Failed to send HELLO to client" << std::endl;
		return -1;
	}
	#endif
	return 0;
}

int keyex(int cd, RSA &rsa, ssh::Keys &keys) {
	char buf[ssh::CLIENT_KEYEX_LEN] = {0};
	if (try_recv(cd, buf, ssh::CLIENT_KEYEX_LEN) == -1)
		return -1;
	#ifndef NDEBUG
	std::cout << "recvd keyex" << std::endl;
	#endif

	ssh::ServerDiffieKeys diffieKeys(buf, rsa);
	std::string check(buf, 768);
	std::cerr << check << std::endl;
	if (send(cd, diffieKeys.pubKeys(), ssh::SERVER_KEYEX_LEN, 0) != ssh::SERVER_KEYEX_LEN) {
		perror("ERROR: Failed to send keys.");
		return -1;
	}

	if (diffieKeys.genKeys(keys) == -1)
		return -1;
	
	return 0;
}

int bank(int cd, const ssh::Keys &keys) {
	char recvbuf[ssh::TOTAL_LEN] = {0};
	ssize_t recvlen;
	if ((recvlen = try_recv(cd, recvbuf, ssh::TOTAL_LEN)) == -1)
		return -1;
	ssh::RecvMsg msg(recvbuf, recvlen, keys);
	if (msg.error)
		msg.type = ssh::MsgType::INVALID;

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
		msgAmt = safe[msg.uid] += msg.amt;
		#ifndef NDEBUG
		std::cout << "New Balance for " << (int)msg.uid << " is " << safe[msg.uid] << std::endl;
		#endif
	} break;
	case ssh::MsgType::WITHDRAW: {
		if (msg.amt > safe[msg.uid]) {
			std::cout << "Client attempted to withdraw more than they had." << std::endl;
			msgType = ssh::MsgType::NOT_ENOUGH_DOUGH;
			break;
		}
		msgAmt = safe[msg.uid] -= msg.amt;
		#ifndef NDEBUG
		std::cout << "New Balance for " << (int)msg.uid << " is " << safe[msg.uid] << std::endl;
		#endif
	} break;
	case ssh::MsgType::BALANCE: {
		msgAmt = safe[msg.uid];
		#ifndef NDEBUG
		std::cout << "Current balance for " << (int)msg.uid << " is " << msgAmt << std::endl;
		#endif
	} break;
	default: {
		#ifndef NDEBUG
		if (msg.error)
			std::cout << msg.error << std::endl;
		else
		#else
		std::cout << "Invalid message format" << std::endl;
		#endif
		msgType = ssh::MsgType::BAD_FORMAT;
	}
	}

	if (send(cd, ssh::SendMsg(msgType, msg.uid, msgAmt, keys) , ssh::TOTAL_LEN, 0) != ssh::TOTAL_LEN) {
		perror("ERROR: Failed to send message");
		return -1;
	}
	
	return 0;
}

int serve(int cd) {
	RSA rsa;
	rsa.LoadKeys("serverKeys");
	ssh::Keys keys;
	if (hello(cd, rsa) == -1 
			|| keyex(cd, rsa, keys) == -1 
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
