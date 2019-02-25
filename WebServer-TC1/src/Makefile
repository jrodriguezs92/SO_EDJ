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

APP=WebServer
CC=gcc
LIBS=../lib
INCLUDE=../include
LINKERFLAGS=-lserver
OUT=../bin

all: $(APP)

$(APP).o:$(APP).c
		$(CC) -c $(APP).c -o $(APP).o -I$(INCLUDE)

$(APP):$(APP).o
		$(CC) -o $(OUT)/$(APP) $(APP).o -L$(LIBS) $(LINKERFLAGS)

clean:
		rm -f  $(OUT)/$(APP)*
		rm -f *.o 
		rm -f *.so
		rm -f *.a

.PHONY:clean