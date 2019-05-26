KERNELDIR := /lib/modules/$(shell uname -r)/build

PWD :=$(shell pwd)

obj-m := arduino_driver.o

default:
	$(MAKE) -C $(KERNELDIR)  M=$(PWD) 
	
clean:
		rm -f *.o
		rm -f *.ko
		rm -f *.mod.c
		rm -f *.symvers
		rm -f *.order
		rm -rf .*

.PHONY: clean