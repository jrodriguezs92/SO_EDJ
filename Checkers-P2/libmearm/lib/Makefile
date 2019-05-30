#********************************************************
#			Instituto Tecnológico de Costa Rica
#				Computer Engineering
#
#		Programmer: Esteban Agüero Pérez (estape11)
#		Programming Language: C
#		Version: 1.0
#		Last Update: 30/05/2019
#
#				Operating Systems Principles
#				Professor. Diego Vargas
#********************************************************

LIB1=meArmSerial
LIB2=roboticPlayer
CC=gcc
INCLUDE=../include
AR=ar
OPTNS=-Wall

all:lib$(LIB1).a lib$(LIB2).a

# Creates the object files
$(LIB1).o:$(LIB1).c
		$(CC) $(OPTNS) -c $(LIB1).c -o $(LIB1).o -I$(INCLUDE)

# Generates the library
lib$(LIB1).a:$(LIB1).o
		$(AR) rcs lib$(LIB1).a $(LIB1).o

# Creates the object files
$(LIB2).o:$(LIB2).c
		$(CC) $(OPTNS) -c $(LIB2).c -o $(LIB2).o -I$(INCLUDE)

# Generates the library
lib$(LIB2).a:$(LIB2).o
		$(AR) rcs lib$(LIB2).a $(LIB1).o $(LIB2).o

clean:
		rm -f *.o 
		rm -f *.a
		rm -f *_wrap.c
		
.PHONY:clean