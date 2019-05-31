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

#include <meArmSerial.h>

int meArmInit(const char * serialPort, int baud ) {
	struct termios toptions;
	int fd;

	fd = open(serialPort, O_RDWR | O_NONBLOCK );

	if (fd == -1)  {
		perror("meArmInit: Unable to open port");
		return -1;

	}

	if (tcgetattr(fd, &toptions) < 0) {
		perror("meArmInit: Couldn't get term attributes");
		return -1;

	}

	speed_t brate = baud;
	switch(baud) {
		case 4800:   brate=B4800;   break;
		case 9600:   brate=B9600;   break;
#ifdef B14400
		case 14400:  brate=B14400;  break;
#endif
		case 19200:  brate=B19200;  break;
#ifdef B28800
		case 28800:  brate=B28800;  break;
#endif
		case 38400:  brate=B38400;  break;
		case 57600:  brate=B57600;  break;
		case 115200: brate=B115200; break;

	}
	cfsetispeed(&toptions, brate);
	cfsetospeed(&toptions, brate);
	toptions.c_cflag &= ~PARENB;
	toptions.c_cflag &= ~CSTOPB;
	toptions.c_cflag &= ~CSIZE;
	toptions.c_cflag |= CS8;

	// no flow control
	toptions.c_cflag &= ~CRTSCTS;
	toptions.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
	toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl
	toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
	toptions.c_oflag &= ~OPOST; // make raw

	// termios
	toptions.c_cc[VMIN]  = 0;
	toptions.c_cc[VTIME] = 0;
	tcsetattr(fd, TCSANOW, &toptions);

	if( tcsetattr(fd, TCSAFLUSH, &toptions) < 0) {
		perror("meArmInit: Couldn't set term attributes");
		return -1;

	}

	return fd;

}

int meArmClose(int fd) {
	return close(fd);

}

int meArmWriteByte(int fd, uint8_t b) {
	int n = write(fd,&b,1);
	if( n!=1) {
		return -1;

	}

	return 0;

}

int meArmWrite(int fd, const char * str) {
	int len = strlen(str);
	int n = write(fd, str, len);
	if( n!=len ) {
		perror("meArmWrite: Couldn't write whole string\n");
		return -1;

	}
	//sleep(3); // to wait the arduino to perform the action
	return 0;

}

int meArmReadUntil(int fd, char * buf, char until, int bufMax, int timeout) {
	char b[1];  // read expects an array, so we give it a 1-byte array
	int i=0;
	do { 
		int n = read(fd, b, 1);  // read a char at a time
		if( n==-1) {
			return -1;    // couldn't read

		}

		if( n==0 ) {
			usleep( 1 * 1000 );  // wait 1 msec try again
			timeout--;
			if( timeout==0 ) {
				return -2;

			}

			continue;

		}
#ifdef SERIALPORTDEBUG  
		printf("meArmReadUntil: i=%d, n=%d b='%c'\n", i, n, b[0]); // debug
#endif
		buf[i] = b[0]; 
		i++;
	} while( b[0] != until && i < bufMax && timeout > 0 );

	buf[i] = 0;  // null terminate the string

	return 0;

}

int meArmReadFlush(int fd) {
	sleep(2); //required to make flush work
	return tcflush(fd, TCIOFLUSH);

}
