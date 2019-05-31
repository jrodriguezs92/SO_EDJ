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

DRIVER=arduino_mearm
KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD :=$(shell pwd)
obj-m := $(DRIVER).o

default:
		$(MAKE) -C $(KERNELDIR)  M=$(PWD) 

install: default
		sudo insmod $(DRIVER).ko

uninstall:
		sudo rmmod $(DRIVER)

uninstall_others:
		sudo rmmod ch341
	
clean:
		rm -f *.o
		rm -f *.ko
		rm -f *.mod.c
		rm -f *.symvers
		rm -f *.order
		rm -rf .tmp_versions
		rm -rf .arduino*
		rm -rf .cache*
		clear

.PHONY: clean default uninstall