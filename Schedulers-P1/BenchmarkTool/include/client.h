/*client.h*/

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

#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

// Threads Library
#include <mypthread.h>

#define BUFFER 1024
#define NUMBER_OF_STRING 1000
#define MAX_STRING_SIZE 10000

int n_cycles, port;
char *host, *file;
char initialTime[NUMBER_OF_STRING][MAX_STRING_SIZE], finalTime[NUMBER_OF_STRING][MAX_STRING_SIZE];

void *sendRequest(void*);

/*client.h*/
