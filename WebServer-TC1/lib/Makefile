#************************************************************
#	Instituto Tecnológico de Costa Rica
#	Ingeniería en Computadores
#
#	Programador: Esteban Agüero Pérez (estape11)
#
#	Última Modificación: 24/02/2019
#
#	Principios de Sistemas Operativos
#	Prof. Diego Vargas
#
#************************************************************

APP=server
CC=gcc
INCLUDE=../include
AR=ar

all:lib$(APP).a

$(APP).o:$(APP).c
		$(CC) -c $(APP).c -o $(APP).o -I$(INCLUDE)

lib$(APP).a:$(APP).o
		$(AR) rcs lib$(APP).a $(APP).o

clean:
		rm -f *.o 
		rm -f *.a
		
.PHONY:clean