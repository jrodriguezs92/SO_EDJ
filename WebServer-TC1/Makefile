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