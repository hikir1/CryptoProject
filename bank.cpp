#include <iostream>
#include <cstdlib>
#include <cerrno>
#include <cstdint>
#include <stdio.h>

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

int imtired(int cd) {
#if 0
	constexpr unsigned int TIMEOUT = 3; // seconds
	alarm(TIMEOUT);
#endif

	return 0; // SUCCESS
}

void timeout(int signum) {}

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
			close(cd);
			close(sd);
			return EXIT_FAILURE;
		}
		if(close(cd) == -1) {
			perror("ERROR: Failed to close client socket");
			close(sd);
			return EXIT_FAILURE;
		}
	}
	

}
