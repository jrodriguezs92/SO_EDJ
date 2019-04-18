#***************************************************
#
#	Instituto Tecnológico de Costa Rica
#	Computer Engineering
#
#	Programmer: Esteban Agüero Pérez (estape11)
#
#	Last update: 18/04/2019
#
#	Operating Systems Principles
#	Professor. Diego Vargas
#
#***************************************************

all: src

lib:
		$(MAKE) -C lib
src: lib	
		$(MAKE) -C src

clean:
		$(MAKE) -C lib clean
		$(MAKE) -C src clean
		clear

.PHONY: all lib src clean