 /* meArmSerial.i */
 %module meArmSerial
 %{
	/* Put header files here or function declarations like below */
	extern int meArmInit(const char*, int );
	extern int meArmClose(int);
	extern int meArmWriteByte(int, uint8_t);
	extern int meArmWrite(int, const char *);
	extern int meArmReadUntil(int, char *, char, int, int);
	extern int meArmReadFlush(int);
 %}

extern int meArmInit(const char*, int );
extern int meArmClose(int);
extern int meArmWriteByte(int, uint8_t);
extern int meArmWrite(int, const char *);
extern int meArmReadUntil(int, char *, char, int, int);
extern int meArmReadFlush(int);