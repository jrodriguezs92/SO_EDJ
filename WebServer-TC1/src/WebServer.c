/*
*******************************************************************

				Instituto Tecnológico de Costa Rica
					Computer Engineering

		Programmer: Esteban Agüero Pérez (estape11)
		Programming Language: C
		Version: 1.0
		Last Update: 26/02/2019

		Inputs: Port and/ or root directory
		Restrictions: Numeric Value / String

					Operating Systems Principles
					Professor. Diego Vargas

*******************************************************************
*/

#include <server.h>

int main(int argc, char* argv[]){

	struct sockaddr_in clienteAddr;
	socklen_t addrLen;
	char flag;    
	
	// default root = ~/res and port=10101
	char port[6];
	dirRoot = getenv("PWD"); // gets the current directory
	strcat(dirRoot, "/res"); // $(PWD)/res
	strcpy(port,"10101"); // port=10101

	int slot=0;

	// parse the arguments
	while ((flag = getopt (argc, argv, "p:r:")) != -1)
		switch (flag) {
			case 'r':
				dirRoot = malloc(strlen(optarg));
				strcpy(dirRoot,optarg);
				break;
			case 'p':
				strcpy(port,optarg);
				break;
			case '?':
				fprintf(stderr,"Wrong argument\n");
				exit(1);
			default:
				exit(1);
		}
	
	printf("> Server started\n\t Port: %s%s%s \n\t Root directory: %s%s%s\n","\033[92m",
			port,"\033[0m","\033[92m",dirRoot,"\033[0m");

	// set all the clients in -1, means theyre not connected
	int i;
	for (i=0; i<CONEXMAX; i++) {
		clients[i]=-1;

	}

	startServer(port);

	// loop to accept new connections
	while (1) {
		addrLen = sizeof(clienteAddr);
		// system call to create new socket connection
		clients[slot] = accept (sockfd, (struct sockaddr *) &clienteAddr, &addrLen);

		if (clients[slot]<0){
			perror("accept() error");

		}

		else {
			requestResponse(slot); // serve one request at the time
			/*
			// serve more then one request at the time
			if ( fork()==0 ){
				requestResponse(slot);
				exit(0);

			}
			*/
		}

		while (clients[slot]!=-1) {
			slot = (slot+1)%CONEXMAX; 

		}
	}
	
	return 0;
}