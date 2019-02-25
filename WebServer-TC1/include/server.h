/*server.h*/

/*
*******************************************************************

				Instituto Tecnológico de Costa Rica
					Ingeniería en Computadores

		Programador: Esteban Agüero Pérez (estape11)
		Lenguaje: C++
		Versión: 1.0
		Última Modificación: 24/02/2019

					Principios de Sistemas Operativos
						Prof. Diego Vargas

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
int sockfd, clientes[CONEXMAX]; // fd=file descriptor
void iniciarServidor(char *);
void responderSolicitud(int);

/*server.h*/