/*server.c*/

/*
*******************************************************************

				Instituto Tecnológico de Costa Rica
					Computer Engineering

		Programmer: Esteban Agüero Pérez (estape11)
		Programming Language: C
		Version: 1.0
		Last Update: 26/02/2019

					Operating Systems Principles
					Professor. Diego Vargas

*******************************************************************
*/

#include <server.h>

void startServer(char* puerto) {
	struct addrinfo hints, *res, *p;

	// getaddrinfo for the host
	memset (&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo( NULL, puerto, &hints, &res) != 0) {
		perror ("getaddrinfo() error");
		exit(1);

	}

	// socket and bind
	for (p = res; p!=NULL; p=p->ai_next){
		// creates the connection point
		sockfd = socket (p->ai_family, p->ai_socktype, 0); 

		if (sockfd == -1) {
			continue;

		}

		// sets the socket addres
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == 0) { 
			break;

		}

	}
	if (p==NULL) { // case connection can be done
		perror ("socket() or bind()");
		exit(1);

	}

	freeaddrinfo(res); // release mem to avoit memory leaks

	// listening new connections
	if ( listen (sockfd, 1000000) != 0 ) {
		perror("listen() error");
		exit(1);

	}

	//fcntl(sockfd, F_SETFL, O_NONBLOCK); // change the socket into non-blocking state

}

void requestResponse(int n) {
	printf("\n** Start communication with %i **\n",n);
	char message[MSGLEN], *reqline[3], data_to_send[BYTES], path[MSGLEN];
	int rcvd, fd, bytesLeidos;
	memset( (void*) message, (int)'\0', MSGLEN );

	// 5s timeout
	struct timeval tv = {5, 0};
	setsockopt(clientes[n], SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv, sizeof(struct timeval));

	rcvd=recv(clientes[n], message, MSGLEN, 0);

	if (rcvd<0) {    // receive an error
		fprintf(stderr,("recv() error\n"));

	}

	else if (rcvd==0) {    // socket closed
		fprintf(stderr,"> Client disconnected.\n");

	}

	else if((strcmp(message, "\n")) != 0){    // message received
		printf("Message received: \n%s", message);
		reqline[0] = strtok (message, " \t\n");

		if ( strncmp(reqline[0], "GET\0", 4)==0 ){
			reqline[1] = strtok (NULL, " \t");
			reqline[2] = strtok (NULL, " \t\n");
			if ( strncmp( reqline[2], "HTTP/1.1", 8)!=0 )	{
				write(clientes[n], "HTTP/1.1 400 Bad Request\n", 25);

			}

			else {
				if ( strncmp(reqline[1], "/\0", 2)==0 ) {
					reqline[1] = "/index.html"; // defaul file to show
				}

				strcpy(path, dirRoot);
				strcpy(&path[strlen(dirRoot)], reqline[1]);
				printf("Enviando: %s\n", path);

				if ( (fd=open(path, O_RDONLY))!=-1 ) { // file found
					send(clientes[n], "HTTP/1.1 200 OK\n\n", 17, 0);

					while ( (bytesLeidos=read(fd, data_to_send, BYTES))>0 ) {
						write (clientes[n], data_to_send, bytesLeidos);

					}

				}
				else { // file not found
					write(clientes[n], "HTTP/1.1 404 Not Found\n", 23);

				}
			}
		}
	}

	// closing socket
	close(clientes[n]);
	clientes[n]=-1;
	printf("\n** End communication with %i **\n",n);
}

/*server.c*/