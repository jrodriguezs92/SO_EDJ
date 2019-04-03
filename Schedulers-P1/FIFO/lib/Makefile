#************************************************************
#	Instituto Tecnológico de Costa Rica
#	Computer Engineering
#
#	Programmer: Esteban Agüero Pérez (estape11)
#
#	Last update: 28/03/2019
#
#	Operating Systems Principles
#	Professor. Diego Vargas
#
#************************************************************

LIB_SERVER=server
LIB_PHP=ph7
CC=gcc
INCLUDE=../include
AR=ar
OPTNS=-Wall

all:lib$(LIB_SERVER).a lib$(LIB_PHP).a

$(LIB_SERVER).o:$(LIB_SERVER).c
		$(CC) $(OPTNS) -c $(LIB_SERVER).c -o $(LIB_SERVER).o -I$(INCLUDE)

# Make the library for the server
lib$(LIB_SERVER).a:$(LIB_SERVER).o
		$(AR) rcs lib$(LIB_SERVER).a $(LIB_SERVER).o

$(LIB_PHP).o:$(LIB_PHP).c
		$(CC) $(OPTNS) -c $(LIB_PHP).c -o $(LIB_PHP).o -I$(INCLUDE)

# Make the library for the php interpreter
lib$(LIB_PHP).a:$(LIB_PHP).o
		$(AR) rcs lib$(LIB_PHP).a $(LIB_PHP).o

clean:
		rm -f *.o 
		rm -f *.a
		
.PHONY:clean