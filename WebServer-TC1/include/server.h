/*server.h*/

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

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<signal.h>
#include<fcntl.h>

#define CONEXMAX 1000
#define BYTES 1024
#define MSGLEN 9999

char* dirRoot;
int sockfd, clients[CONEXMAX]; // fd=file descriptor
void startServer(char *);
void requestResponse(int);

/*server.h*/