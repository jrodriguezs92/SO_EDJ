#********************************************************
#			Instituto Tecnológico de Costa Rica
#				Computer Engineering
#
#		Programmer: Esteban Agüero Pérez (estape11)
#		Version: 1.0
#		Last Update: 30/05/2019
#
#				Operating Systems Principles
#				Professor. Diego Vargas
#********************************************************

all: src python

lib:
		$(MAKE) -C lib

src: lib	
		$(MAKE) -C src

python:	
		$(MAKE) -C python

clean:
		$(MAKE) -C lib clean
		$(MAKE) -C src clean
		$(MAKE) -C python clean
		clear

.PHONY: all lib src python clean