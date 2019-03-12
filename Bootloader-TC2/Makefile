#************************************************************
#	Instituto Tecnológico de Costa Rica
#	Computer Engineering
#
#	Programmer: Esteban Agüero Pérez (estape11)
#
#	Last update: 12/03/2019
#
#	Operating Systems Principles
#	Professor. Diego Vargas
#
#************************************************************

APP=tetris
COMPILER=nasm
FLAGS= -f bin
USB= ""

all: tetris.img

bootloader.bin:bootloader.asm
		$(COMPILER) $(FLAGS) bootloader.asm -o bootloader.bin

$(APP).bin:$(APP).asm
		$(COMPILER) $(FLAGS) tetris.asm -o tetris.bin

$(APP).img: $(APP).bin bootloader.bin
		dd if=/dev/zero of=$(APP).img bs=1024 count=512
		dd if=bootloader.bin of=$(APP).img conv=notrunc
		dd if=$(APP).bin of=$(APP).img bs=512 seek=1 conv=notrunc

clean:
		rm -f *.bin
		rm -f *.img
		clear

tetrisWIP.img: bootloader.bin
		tasm $(APP)WIP.asm, $(APP)WIP.bin
		dd if=/dev/zero of=$(APP)WIP.img bs=1024 count=512
		dd if=bootloader.bin of=$(APP)WIP.img conv=notrunc
		dd if=$(APP)WIP.bin of=$(APP)WIP.img bs=512 seek=1 conv=notrunc


emu: $(APP).img
		qemu-system-i386 -fda $(APP).img

install: $(APP).img
ifeq ($(USB),"")
		@echo "Error: USB not defined"
else
		sudo dd if=$(APP).img of=$(USB)
endif

.PHONY=clean
