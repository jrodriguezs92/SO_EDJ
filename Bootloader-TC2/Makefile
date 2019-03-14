#************************************************************
#	Instituto Tecnológico de Costa Rica
#	Computer Engineering
#
#	Programmers: Esteban Agüero Pérez
#				 Jeremy Rodriguez Solorzano
#				 Daniela Hernandez Alvarado
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
COND=Funci
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

$(APP)$(COND).img:
		nasm -f bin $(APP)$(COND).asm -o $(APP)$(COND).bin
		dd if=/dev/zero of=$(APP)$(COND).img bs=1024 count=512
		dd if=bootloader.bin of=$(APP)$(COND).img conv=notrunc
		dd if=$(APP)$(COND).bin of=$(APP)$(COND).img bs=512 seek=1 conv=notrunc
		qemu-system-i386 -fda $(APP)$(COND).img


emu: $(APP).img
		qemu-system-i386 -fda $(APP).img

install: $(APP).img
ifeq ($(USB),"")
		@echo "Error: USB not defined"
else
		sudo dd if=$(APP).img of=$(USB)
endif

.PHONY=clean tetrisWIP.img
