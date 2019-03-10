; HELLO WORLD FROM 32 BIT PROTECTED MODE LOADING 512 BYTES FROM SECOND SECTOR INTO MEMORY
;------------------------------------------------------------------------------------------------------------

bits 16						;Working on 16 bits mode
org 0x7c00					;Set the assembler location counter at this direction to specify the access to RAM

boot:
	;Enable A20 line to acces more memory:
	mov ax, 0x2401			
	int 0x15				
	
	;Set VGA to text mode:
	mov ax, 0x3
	int 0x10

	;Disk number placed on memory:
	mov [disk],dl
	
	;For write programs larger than 512 bytes load more off the disk:
	mov ah, 0x2    ;read sectors
	mov al, 1      ;sectors to read
	mov ch, 0      ;cylinder idx
	mov dh, 0      ;head idx
	mov cl, 2      ;sector idx
	mov dl, [disk] ;disk idx
	mov bx, copy_target ;target pointer
	int 0x13 ;Taget location

	cli	;Clear interrupt flag

	;Enable 32 bit instructions
	;Set up Global descriptor table (GDT): contains entries telling the CPU about memory segments
	lgdt [gdt_pointer] ;Load GDT table
	mov eax, cr0
	;Set the protected mode bit on special CPU register
	or eax,0x1 
	mov cr0, eax

	mov ax, DATA_SEG
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp CODE_SEG:boot2 ;jump to the code segment

	;Define GDT
gdt_start:
	dq 0x0
gdt_code:
	dw 0xFFFF
	dw 0x0
	db 0x0
	db 10011010b
	db 11001111b
	db 0x0
gdt_data:
	dw 0xFFFF
	dw 0x0
	db 0x0
	db 10010010b
	db 11001111b
	db 0x0
gdt_end:

;Load GDT with a gdt pointer structure
gdt_pointer:
	dw gdt_end - gdt_start
	dd gdt_start
disk:
	db 0x0

;Offsets into GDT for use later
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

;Define the storage space for boot loader into the device
times 510 - ($-$$) db 0
dw 0xaa55
copy_target:

;Get into 32 bit mode
bits 32

	hello: db "Hello more than 512 bytes world!!",0

;Program loaded from beyond the first 512 bytes
boot2:
	mov esi,hello
	mov ebx,0xb8000
.loop:
	lodsb
	or al,al
	jz halt
	or eax,0x0F00
	mov word [ebx], ax
	add ebx,2
	jmp .loop
halt:
	cli
	hlt

;Pads the bootloader to 1024 bytes. For not copy uninitialised bytes from disk
times 1024 - ($-$$) db 0