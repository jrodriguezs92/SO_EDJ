KERNELDIR := /lib/modules/$(shell uname -r)/build

PWD :=$(shell pwd)

obj-m := arduino_driver.o

default:
		$(MAKE) -C $(KERNELDIR)  M=$(PWD) 

install: default
		sudo insmod arduino_driver.ko

uninstall:
		sudo rmmod arduino_driver

uninstall_others:
		sudo rmmod ch341
	
clean:
		rm -f *.o
		rm -f *.ko
		rm -f *.mod.c
		rm -f *.symvers
		rm -f *.order
		rm -rf .*

.PHONY: clean uninstall