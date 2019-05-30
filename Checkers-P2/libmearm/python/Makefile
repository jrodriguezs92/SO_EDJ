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
LIBDIR=../lib
PYTHON=/usr/include/python3.5

all:_$(LIB1).so _$(LIB2).so

# Bring the .c files
$(LIB1).c:$(LIBDIR)/$(LIB1).c
		cp $(LIBDIR)/$(LIB1).c $(LIB1).c

$(LIB2).c:$(LIBDIR)/$(LIB2).c
		cp $(LIBDIR)/$(LIB2).c $(LIB2).c

# Wraps the C file into Python
$(LIB1)_wrap.c: $(LIB1).i $(LIB1).c
		swig -python $(LIB1).i

$(LIB2)_wrap.c: $(LIB2).i $(LIB2).c
		swig -python $(LIB2).i

_$(LIB1).so:$(LIB1)_wrap.c
		$(CC) -fPIC -c $(LIB1).c $(LIB1)_wrap.c -I$(PYTHON) -I$(INCLUDE)
		ld -shared $(LIB1).o $(LIB1)_wrap.o -o _$(LIB1).so
		rm -rf $(LIB1).c

_$(LIB2).so:$(LIB2)_wrap.c
		$(CC) -fPIC -c $(LIB2).c $(LIB2)_wrap.c -I$(PYTHON) -I$(INCLUDE)
		ld -shared $(LIB2).o $(LIB2)_wrap.o -o _$(LIB2).so
		rm -rf $(LIB2).c

clean:
		rm -rf *.o
		rm -rf *.so
		rm -rf *_wrap.c
		rm -rf _*
		rm -rf *.py

.PHONY: all clean
