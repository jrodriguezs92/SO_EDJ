 /* meArmSerial.i */
 %module roboticPlayer
 %{
	/* Put header files here or function declarations like below */
	
	extern char* confFileName;
	extern char* appName;
	extern char* cmmd;
	extern float argX;
	extern float argY;

	extern int readConfFile(void);
	extern int paser (char *);
	extern char * trimValue(char *);
	extern int isNumber (const char *);

	extern void move(float,float);
	extern void pick(void);
	extern void drop(void);
	extern void moveandpick(float,float);
	extern void moveanddrop(float,float);
 %}


extern char* confFileName;
extern char* appName;
extern char* cmmd;
extern float argX;
extern float argY;

extern int readConfFile(void);
extern int paser (char *);
extern char * trimValue(char *);
extern int isNumber (const char *);

extern void move(float,float);
extern void pick(void);
extern void drop(void);
extern void moveandpick(float,float);
extern void moveanddrop(float,float);