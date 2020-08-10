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
	alarm(TIMEOUT);
	ssize_t len = recv(cd, buf, buflen - 1, 0);
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

int keyex(int cd, unsigned char id, Keys &keys) {
	char buf[KEYEX_LEN + 1] = {0};
	if (try_recv(cd, buf, KEYEX_LEN + 1) == -1)
		return -1;

	// TODO: RSA Decrypt

	// TODO: Parse client key parts
	// TODO: gen keys

	// TODO: RSA Encrypt server key parts
	// TODO: send server key parts
	
	return 0;
}

int bank(int cd, const Keys &keys) {
	char mac[hmac::output_length + 1];
	char ctxt[MSG_MAX + aes::BLOCK_BYTES + 1];
	ssize_t ctxtlen;
	if (try_recv(cd, mac, hmac::output_length + 1) == -1
			|| (ctxtlen = try_recv(cd, ctxt, MSG_MAX + 1)) == -1)
		return -1;
	if (ctxtlen % aes::BLOCK_BYTES != 0) {
		std::cout << "Invalid message format." << std::endl;
		return -1;
	}
	char ptxt[sizeof(ctxt)];
	aes::cbc_decrypt(ctxt, ptxt, (size_t)ctxtlen, keys.aes_iv, keys.aes_key);
	if (hmac::create_HMAC(ptxt, keys.hmac_key).compare(mac) != 0) {
		std::cout << "Corrupt message detected. HMAC's do not match." << std::endl;
		return -1;
	}

	// put server's ptxt in ctxt

	switch ((int)ptxt[0]) {
	case BankMsg::DEPOSIT:
		ctxt[0] = 0;
	break;
	case BankMsg::WITHDRAW:
		ctxt[0] = 0;
	break;
	case BankMsg::BALANCE:
		ctxt[0] = 0;
	break;
	default:
	break;
	}

	// server's ptxt is now in ctxt
	// server's ctxt will be put in ptxt

	if (send(cd, hmac::create_HMAC(ctxt, keys.hmac_key).c_str(),
			hmac::output_length, MSG_MORE) == -1) {
		perror("ERROR: Failed to send HMAC");
		return -1;
	}
	size_t newlen = (size_t)ctxtlen;
	aes::cbc_encrypt(ctxt, ptxt, newlen, keys.aes_iv, keys.aes_key);
	if (send(cd, ptxt, newlen, 0) == -1) {
		perror("ERROR: Failed to send message");
		return -1;
	}
	
	
	return 0;
}

int imtired(int cd) {

	return 0;
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
		if (imtired(cd) == -1) {
			shutdown(cd, SHUT_RDWR);
			close(cd);
			close(sd);
			return EXIT_FAILURE;
		}
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
