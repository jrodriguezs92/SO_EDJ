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

bits 16						; Define to nasm that we are running in real mode
org 0x7C00  				; Physical address of the bootloader

init:
	XOR AX, AX				; Reset AX
	MOV DS, AX  			; DS = 0

	CLI        				; Turn off interrupts

	MOV SS, AX 				; SS = 0x0000
	MOV SP, 0x7C00			; SP = 0x7c00

	STI          			; Turn on interrupts

	XOR AX, AX   			; Reset AXr
	INT 0x13
	JC init        			; If failed, re-run init again

	MOV AX, 0x1000			; When we read the sector, reads from address 0x1000
	MOV ES, AX     			; Set ES with 0x1000

prepare_game:				; Set up to read the second stage (the game)
	XOR BX, BX   			; Reset value of register to ensure that the buffer offset is 0
	MOV AH, 0x2  			; 2 = Read USB drive
	MOV AL, 0x8  			; Read eight sectors
	MOV CH, 0x0  			; Track 1
	MOV CL, 0x2  			; Sector 2, track 1
	MOV DH, 0x0  			; Head 1
	INT 0x13
	JC prepare_game   		; If failed, re-run prepare_game

	JMP 0x1000:0000 		; Otherwise jumps to Tetris game address

TIMES 510 - ($ - $$) DB 0	; Fill the rest of the sector with zeros
DW 0xAA55   				; To make it bootable
