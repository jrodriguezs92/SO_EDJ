;************************************************************
;	Instituto Tecnológico de Costa Rica
;	Computer Engineering
;
;	Programmer: Esteban Agüero Pérez (estape11)
;
;	Last update: 12/03/2019
;
;	Operating Systems Principles
;	Professor. Diego Vargas
;
;************************************************************

bits 16							; Define to nasm that we are running in real mode
org 0x7C00  					; Physical address of the bootloader

init:
	mov ax, 07C0h				; Set up 4K stack space after this bootloader
	add ax, 288					; (4096 + 512) / 16 bytes per paragraph
	mov ss, ax
	mov sp, 0x2000				; 8k of stack space.

	mov ax, 07C0h				; Set data segment to where we're loaded
	mov ds, ax

	xor ax, ax   				; Reset AX
	int 0x13
	jc init        				; If failed, re-run init again

	mov ax, 0x1000				; When we read the sector, reads from address 0x1000
	mov es,ax     				; Set ES with 0x1000

prepare_game:					; Set up to read the second stage (the game)
	xor bx, bx   				; Reset value of bx to ensure that the buffer offset is 0
	mov ah, 0x2  				; 2 = Read USB drive
	mov al, 0x10  				; Read eight sectors
	mov ch, 0x0  				; Track 1
	mov cl, 0x2  				; Sector 2, track 1
	mov dh, 0x0  				; Head 1
	int 0x13
	jc prepare_game   			; If failed, re-run prepare_game

	jmp 0x1000:0000 			; Otherwise jumps to Tetris game address

times 510 - ($ - $$) db 0		; Padding the rest of the sector with zeros
dw 0xAA55   					; To make it bootable