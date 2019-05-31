//********************************************************
//			Instituto Tecnológico de Costa Rica
//				Computer Engineering
//
//		Programmer: Esteban Agüero Pérez (estape11)
//		Programming Language: C
//		Version: 1.0
//		Last Update: 30/05/2019
//
//				Operating Systems Principles
//				Professor. Diego Vargas
//********************************************************

#include <stdint.h>		// Standard types 
#include <stdio.h>		// Standard input/output definitions 
#include <unistd.h>		// UNIX standard function definitions 
#include <fcntl.h>		// File control definitions 
#include <errno.h>		// Error number definitions 
#include <termios.h>	// POSIX terminal control definitions 
#include <string.h>		// String function definitions 
#include <sys/ioctl.h>	// I/O control definitions

// Initialize the device, returns the fd
// @author estape11
// @params serialport, baud
// @return fd
int meArmInit(const char*, int );

// Close the file descriptor
// @author estape11
// @params fd
// @return int
int meArmClose(int);

// Writes a byte to device
// @author estape11
// @params fd, b
// @return int
int meArmWriteByte(int, uint8_t);

// Writes a string to device
// @author estape11
// @params fd, str
// @return int
int meArmWrite(int, const char *);

// Reads the device until a char
// @author estape11
// @params fd, buf, until, bufMax, timeout
// @return int
int meArmReadUntil(int, char *, char, int, int);

// Flush the read buffer
// @author estape11
// @params fd
// @return int
int meArmReadFlush(int);
