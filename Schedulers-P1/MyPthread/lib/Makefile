#************************************************************
#
#	Instituto Tecnológico de Costa Rica
#	Computer Engineering
#
#	Programmer: Esteban Agüero Pérez (estape11)
#
#	Last update: 03/04/2019
#
#	Operating Systems Principles
#	Professor. Diego Vargas
#
#************************************************************

LIB_NAME=mypthread
CC=gcc
INCLUDE=../include
AR=ar
OPTNS=-Wall

all:lib$(LIB_NAME).a

$(LIB_NAME).o:$(LIB_NAME).c
		$(CC) $(OPTNS) -c $(LIB_NAME).c -o $(LIB_NAME).o -I$(INCLUDE)

# Make the library
lib$(LIB_NAME).a:$(LIB_NAME).o
		$(AR) rcs lib$(LIB_NAME).a $(LIB_NAME).o

clean:
		rm -f *.o 
		rm -f *.a
		
.PHONY:clean
