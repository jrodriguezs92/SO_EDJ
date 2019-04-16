/*client.c*/

/*
*******************************************************************

				Instituto Tecnológico de Costa Rica
					Computer Engineering

		Programmer: Esteban Agüero Pérez (estape11)
		Programming Language: C
		Version: 1.0
		Last Update: 02/04/2019

					Operating Systems Principles
					Professor. Diego Vargas

*******************************************************************
*/

#include <client.h>

/*
* Handles of send to the server the requests
*/
void *sendRequest(void* args){
	long id = (long) args; // ID of the thread
	printf("> Executing thread %ld\n", (id+1) );
	// Variables for the connection
	int socket_fd, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char buffer[BUFFER];
	int i;

	// To do the N cycles requests
	for (i = 0; i < n_cycles; i++) {		
		socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (socket_fd < 0) {
			perror("socket()");
	  		exit(1);

		}

		server = gethostbyname(host);
		
		if (server == NULL) {
			printf("gethostbyname()\n");
			exit(1);

		}
			
		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
		serv_addr.sin_port = htons(port);
		
		// Tries to make the connection
		if (connect(socket_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
			perror("connect()");
			exit(1);

		}
		fcntl(socket_fd, F_SETFL, O_NONBLOCK); // non-blocking socket
		// HTML Header
		strcpy(buffer, "GET /");
		strcat(buffer, file);		
		strcat(buffer, " HTTP/1.1\r\nHost: ");
		strcat(buffer, host);
		strcat(buffer, "\r\n\r\n");
		
		// Writes to the server
		if ( (n = write(socket_fd, buffer, BUFFER)) < 0)	{
			perror("write()");
			exit(1);

		}
		
		int sizeRecv, totalSize=0;

		while(true){ // to complete the download
			while((sizeRecv=read(socket_fd, buffer, BUFFER)) == -1 ){	} // waits to reads someting
			if(sizeRecv == 0 ){
				break;
			} else{
				totalSize+= sizeRecv;
			}
			
		}
		printf("> Total received (thread %d) = %d\n", (id+1), totalSize);
		close(socket_fd);
	}

	pthread_exit(NULL);
	return 0;
}

/*client.c*/