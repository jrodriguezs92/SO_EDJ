/*server.h*/

/*
*******************************************************************

				Instituto Tecnológico de Costa Rica
					Computer Engineering

		Programmer: Esteban Agüero Pérez (estape11)
		Programming Language: C
		Version: 1.0
		Last Update: 12/04/2019

					Operating Systems Principles
					Professor. Diego Vargas

*******************************************************************
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>
#include <syslog.h>
#include <getopt.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <ph7.h>

#define MAXLEN 80
#define CONEXMAX 1024*1024
#define BYTES 1024
#define MSGLEN 9999

// global variables

int running;
int delay;
int pidFd;
int startDaemonized;
char* confFileName;
char* pidFileName;
char* appName;
FILE* logStream;
char* logFileName;
int sockfd; // fd=file descriptor
struct sockaddr_in clienteAddr;
socklen_t addrLen;
char* dirRoot;
int* clients;
char* port;
char* scheduler;
int schedulerID;
int customLog;
char logFileTmp[MAXLEN];
char portTmp[MAXLEN];
char rootTmp[MAXLEN];
char schedulerTmp[MAXLEN];
FILE* file;
// function definitions

void startServer(char *);
void requestResponse(int);
int readConfFile(int);
int testConfFile(char *);
void handleSignal(int);
void daemonize();
void printHelp(void);
char* getTime(void);
void fatalError(const char*);
int isPHPRequest(char*);

/*server.h*/