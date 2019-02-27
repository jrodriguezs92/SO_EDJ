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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>
#include <syslog.h>
#include <getopt.h>
#include <errno.h>

#define CONEXMAX 1000
#define BYTES 1024
#define MSGLEN 9999

int running;
int delay;
char *conf_file_name;
char *pid_file_name;
int pid_fd;
char *app_name;
FILE *log_stream;
char *log_file_name;
int start_daemonized;

char* dirRoot;
int sockfd; // fd=file descriptor
int* clients;
struct sockaddr_in clienteAddr;
socklen_t addrLen;

void startServer(char *);
void requestResponse(int);
int read_conf_file(int);
int test_conf_file(char *);
void handle_signal(int);
void daemonize();
void print_help(void);

/*server.h*/