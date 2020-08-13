#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <sys/select.h>


#define BUFFER_SIZE 1024
#define MAX_CLIENTS 32

int TCP(){

}

int UDP(){

}


int main(int argc, char** argv){
	if(argc > 2 || argc < 2){
		fprintf(cerr, "Error: Incorrect cammand line arguments\n");
		return EXIT_FAILURE;
	}
	unsigned short port = (unsigned short) strtoul(argv[1], NULL, 0);
	fd_set readfds;
 	int client_sockets[ MAX_CLIENTS ]; /* client socket fd list */
  	int client_socket_index = 0;  /* next free spot */
  	server.sin_port = htons( port );
	int len = sizeof( server );
	if ( bind( sock, (struct sockaddr *)&server, len ) < 0 ){
		perror( "bind()" );
		exit( EXIT_FAILURE );
	}
	if (listen( sock, MAX_CLIENTS)){
		perror( "listen()" );
		exit( EXIT_FAILURE );
	}



}