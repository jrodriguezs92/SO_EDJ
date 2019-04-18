/*
*******************************************************************

				Instituto Tecnológico de Costa Rica
					Computer Engineering

		Programmer: Esteban Agüero Pérez (estape11)
					Jeremy Rodriguez (jrodriguezs92)

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
#include <time.h>
#include <ctype.h>
#include <ph7.h>
#include <sys/wait.h>

#define CONNMAX 1000
#define BYTES 1024
#define MAXLEN 80

char *dirRoot;
FILE* file;
FILE* logStream;
int listenfd;
int clients[CONNMAX];
char* confFileName;
char logFileTmp[MAXLEN];
char portTmp[MAXLEN];
char rootTmp[MAXLEN];
char schedulerTmp[MAXLEN];
char* logFileName;
char* port;
char* scheduler;
int schedulerID;
char* pidFileName;
int pidFd;
char* appName;
int customLog;
int running;
int startDaemonized;
struct sockaddr_in clienteAddr;
socklen_t addrlen;

void startServer(char *);
void respond(int);
char * trimValue (char *);
int readConfFile(int);
int testConfFile(char *);
char* getTime(void);
void fatalError(const char *);
void handleSignal(int);
void daemonize();
void printHelp(void);
int isPHPRequest(char *);