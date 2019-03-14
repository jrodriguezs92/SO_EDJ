;************************************************************
;	Instituto Tecnológico de Costa Rica
;	Computer Engineering
;
;	Programmers: Esteban Agüero Pérez
;				 Jeremy Rodriguez Solorzano
;				 Daniela Hernandez Alvarado
;
;	Last update: 14/03/2019
;
;	Operating Systems Principles
;	Professor. Diego Vargas
;
;************************************************************

bits 16								;16 bits mode
org 0x0000

;The bootloader jumped to 0x1000:0x0000 which sets CS=0x1000 and IP=0x0000
;We need to manually set the DS register so it can properly find our variables
;NOTE: ax, bx, dx... are general register, so may change a lot, make sure it has 
;the value that its expected

mov ax, cs
mov ds, ax   						;Copy CS to DS

main:
	mov ah, 0x00 					;Set video mode
	mov al, 0x13					;graphics, 320x200 res, 8x8 pixel box
	int 0x10
	mov ah, 0x0c					;Write graphics pixel
	mov bh, 0x00 					;Page #0
	call menu
	call clear 

	mov ah, 0x00 					;Set video mode
	mov al, 0x13					;graphics, 320x200 res, 8x8 pixel box
	int 0x10
	mov ah, 0x0c					;Write graphics pixel
	mov bh, 0x00 					;Page #0

	;Print Title
	pusha
	mov bl, 4
	mov word si, welcomeTitle
	mov dh, 0
	mov dl, 0
	call printMsg
	popa

	;Print hotkeys
	;Title
	pusha
	mov bl, 3
	mov word si, HotKeysTitle
	mov dh, 10
	mov dl, 0
	call printMsg
	popa
	;Left key
	pusha
	mov bl, 15
	mov word si, LeftKey
	mov dh, 12
	mov dl, 0
	call printMsg
	popa
	;Right key
	pusha
	mov bl, 15
	mov word si, RightKey
	mov dh, 14
	mov dl, 0
	call printMsg
	popa
	;Score Board Title
	pusha
	mov bl, 3
	mov word si, ScoreTitle
	mov dh, 2
	mov dl, 27
	call printMsg
	popa
	;Score
	pusha
	mov bl, 15
	mov word si, Score
	mov dh, 2
	mov dl, 34
	call printMsg
	popa
	;Level Title
	pusha
	mov bl, 3
	mov word si, LevelTitle
	mov dh, 4
	mov dl, 27
	call printMsg
	popa
	;Level num
	pusha
	mov bl, 15
	mov word si, levelNumber1
	;cmp word [actualLevel], 1
	;je writeOne
	;call writeOne
	;cmp word [actualLevel], 2
	;je writeTwo
	;call writeTwo
	;cmp word [actualLevel], 3
	;je writeThree
	;call writeThree
	mov dh, 4
	mov dl, 34
	call printMsg
	popa

	mov word [blockUnit], 10 		;Size of block
	mov word [lastColor], 88		;The last color

	mov al, 0x3						;Set dark gray color
	mov cx, 110						;Set column (x) to 110
	mov dx, 10						;Set row (y) to 10
	call drawBoard

	mov word [startX], 150			;Loading starting position
	mov word [startY], 10

	mov word [points], 0			;Clearing points

	;when its generated check the x and y
	mov word cx, [startX]
	mov word dx, [startY]

	mov word [lastPieceX], cx
	mov word [lastPieceY], dx

	;call drawIShape
	;call drawTShape
	;call drawLShape
	;call drawBShape
	call drawZShape

	jmp game 						;Game main loop

writeOne:
	mov word si, levelNumber1
	ret
writeTwo:
	mov word si, levelNumber2
	ret
writeThree:
	mov word si, levelNumber3
	ret

clear:
	mov ah, 0x00
	mov al, 0x13
	int 0x10
	ret

;Draw menu 
menu:
	;Print title 
	pusha
	mov bl, 11
	mov word si,welcomeTitle
	mov dh, 2
	mov dl, 15
	call printMsg
	popa

	;Selection
	pusha
	mov bl, 14
	mov word si, Menu
	mov dh, 7
	mov dl, 2
	call printMsg
	popa

	;Print hot keys 
	;Level 1
	pusha
	mov bl, 3
	mov word si,Level1
	mov dh, 10
	mov dl, 14
	call printMsg
	popa

	;Level 2
	pusha
	mov bl, 10
	mov word si,Level2
	mov dh, 13
	mov dl, 14
	call printMsg
	popa

	;Level 3
	pusha
	mov bl, 13
	mov word si,Level3
	mov dh, 16
	mov dl, 14
	call printMsg
	popa
	
	call getMenuKey
	ret

;Update actualLevel when key 1 pressed
onePressed: 
	mov word [actualLevel], 1		;Loading actual level
	mov word [actualSpeed], 0x00010 ;Set the speed for level 1
	ret

;Update actualLevel when key 2 pressed	
twoPressed:
	mov word [actualLevel], 2		;Loading actual level
	mov word [actualSpeed], 0x0007 ;Set the speed for level 2
	ret

;Update actualLevel when key 3 pressed
threePressed:
	mov word [actualLevel], 3		;Loading actual level
	mov word [actualSpeed], 0x0001 ;Set the speed for level 3
	ret
	

;Draw the board, x value =100, y value =180
;Params:
;		cx = X coordinate
;		dx = Y coordinate
;		al = Color
drawBoard:
	pusha							;Push registers onto the stack
	int 0x10						;Draw initial pixel
	mov bx, cx						;Move initial x position to bx
	add bx, 100						;Add 100 to determine the final position of the block
	call drawLineX					;Draw top horizontal line
	sub cx, 100						;Substract 100 to obtain initial value
	add dx, 180						;Add 180 to determine the position of the down horizontal line
	call drawLineX 					;Draw bottom horizontal line
	sub dx, 180						;Substract 180 to obtain initial value
	sub cx, 100						;Substract 100 to obtain initial value
	mov bx, dx						;Move dx to bx
	add bx, 180						;Add 180 to obtain final value
	call drawLineY					;Draw left vertical line
	add cx, 100						;Add 100 to obtain second vertical line initial position
	sub dx, 180						;Substract 180 to obtain initial value
	call drawLineY					;Draw right vertical line
	popa							;Pops registers from the stack
	ret								;Return

;Horizontal line from cx to bx
;Params:
;		cx = X coordinate
;		bx = length
drawLineX:
	cmp cx, bx 		;Compare if currrent x equals desired x
	je return		;Returns if true
	inc cx			;Increments x coordinate (cx)
	int 0x10 		;Writes graphics pixel
	jmp drawLineX	;Loops to itself


;Vertical line from dx to bx
;Params:
;		dx = Y coordinate
;		bx = length
drawLineY:
	cmp dx, bx 		;Compare if currrent y equals desired y
	je return		;Returns if true
	inc dx			;Increments y coordinate (dx)
	int 0x10 		;Writes graphics pixel
	jmp drawLineY	;Loops to itself

;Return from procedure
return:
	ret

;Restores the color of the last piece
restoreLastColor:
	pusha
	mov word dx, [lastColorCopy]	;Stores the last color copy
	mov word [lastColor], dx		;The last color
	popa
	ret

;Set the color to black (clear)
lastColorToBlack:
	pusha
	mov word dx, [lastColor]		;Stores the las color copy
	mov word [lastColorCopy], dx	;The last color
	mov word [lastColor], 0x0		;The last color to black
	popa
	ret


;=========;=========;=========; Pieces Draws ;=========;=========;=========;

;Draws a square
;Params:
;		cx = X coordinate
;		dx = Y coordinate
;Requires:
;		blockUnit
;		lastColor
drawSquare:
	pusha							;Push registers from the stack
	call drawSquareColor
	popa							;Pop registers from the stack
	ret

drawSquareColor:
	mov ah, 0x0C 					;Write graphics pixel
	mov bx, cx						;Move dx to bx
	add bx, [blockUnit] 			;Define the square size
	mov al, 0						;Set al to 0

drawSquareLoop:
	cmp al, [blockUnit]				;Loop for the size of block
	je return
	inc dx 							;Decrement y coordinate
	pusha							;Push registers to the stack
	mov al, [lastColor]				;Set the color
	call drawLineX
	popa							;Pop registers from the stack
	inc al							;Increment al
	jmp drawSquareLoop				;Loop until finished the square

;Draws the I shape
;Params:
;		cx = X coordinate
;		dx = Y coordinate
drawIShape:
	mov word [lastOrientation], 90	;Saves the orientation
	mov word [lastShape], 1			;Saves the shape
	call drawSquare
	add word dx, [blockUnit]		;Move Y one block down
	call drawSquare
	add word dx, [blockUnit]		;Move Y one block down
	call drawSquare
	add word dx, [blockUnit]		;Move Y one block down
	call drawSquare
	ret

;Clears the I shape
;Params:
;		cx = X coordinate
;		dx = Y coordinate
clearIShape:
	cmp word [lastOrientation], 90
	je clearIShape90
	ret

clearIShape90:
	pusha
	call lastColorToBlack
	call drawIShape
	call restoreLastColor
	popa
	ret

;Draws the L shape
;Params:
;		cx = X coordinate
;		dx = Y coordinate
drawLShape:
	mov word [lastOrientation], 90	;Saves the orientation
	mov word [lastShape], 2			;Saves the shape
	call drawSquare
	add word dx, [blockUnit]		;Move Y one block down
	call drawSquare
	add word dx, [blockUnit]		;Move Y one block down
	call drawSquare
	add word cx, [blockUnit]		;Move X one block righ
	call drawSquare
	ret

;Clears the L shape
;Params:
;		cx = X coordinate
;		dx = Y coordinate
clearLShape:
	cmp word [lastOrientation], 90
	je clearLShape90
	ret

clearLShape90:
	pusha
	call lastColorToBlack
	call drawLShape
	call restoreLastColor
	popa
	ret

;Draws the Z shape
;Params:
;		cx = X coordinate
;		dx = Y coordinate
drawZShape:
	mov word [lastOrientation], 90	;Saves the orientation
	mov word [lastShape], 3			;Saves the shape
	call drawSquare
	add word cx, [blockUnit]		;Move X one block righ
	call drawSquare
	add word dx, [blockUnit]		;Move Y one block down
	call drawSquare
	add word cx, [blockUnit]		;Move X one block righ
	call drawSquare
	ret

;Clears the Z shape
;Params:
;		cx = X coordinate
;		dx = Y coordinate
clearZShape:
	cmp word [lastOrientation], 90
	je clearZShape90
	ret

clearZShape90:
	pusha
	call lastColorToBlack
	call drawZShape
	call restoreLastColor
	popa
	ret

;Draws the B(block) shape
;Params:
;		cx = X coordinate
;		dx = Y coordinate
drawBShape:
	mov word [lastOrientation], 90	;Saves the orientation
	mov word [lastShape], 0			;Saves the shape
	call drawSquare
	add word cx, [blockUnit]		;Move X one block righ
	call drawSquare
	add word dx, [blockUnit]		;Move Y one block down
	call drawSquare
	sub word cx, [blockUnit]		;Move X one block left
	call drawSquare
	ret

;Clears the B(block) shape
;Params:
;		cx = X coordinate
;		dx = Y coordinate
clearBShape:
	cmp word [lastOrientation], 90
	je clearBShape90
	ret

clearBShape90:
	pusha
	call lastColorToBlack
	call drawBShape
	call restoreLastColor
	popa
	ret

;Draws the T shape
;Params:
;		cx = X coordinate
;		dx = Y coordinate
drawTShape:
	mov word [lastOrientation], 90	;Saves the orientation
	mov word [lastShape], 4			;Saves the shape
	call drawSquare
	add word dx, [blockUnit]		;Move Y one block down
	call drawSquare
	add word cx, [blockUnit]		;Move X one block righ
	call drawSquare
	add word dx, [blockUnit]		;Move Y one block down
	sub word cx, [blockUnit]		;Move X one block left
	call drawSquare
	ret

;Clears the T shape
;Params:
;		cx = X coordinate
;		dx = Y coordinate
clearTShape:
	cmp word [lastOrientation], 90
	je clearTShape90
	ret

clearTShape90:
	pusha
	call lastColorToBlack
	call drawTShape
	call restoreLastColor
	popa
	ret


;=========;=========;=========; Pieces Draws ;=========;=========;=========;

;========;========;=========; Pieces Movement ;=========;========;========;

;Compares which shape the last piece has and move it down
moveLastPiece:
	pusha
	mov word cx, [actualSpeed]
	mov dx, 0xA102
	mov ah, 0x86
	int 0x15
	popa
	cmp word [lastShape],0			;Case B(block) Shape
	je moveLastPieceBShape
	cmp word [lastShape],1			;Case I Shape
	je moveLastPieceIShape
	cmp word [lastShape],2			;Case L Shape
	je moveLastPieceLShape
	cmp word [lastShape],3			;Case Z Shape
	je moveLastPieceZShape
	cmp word [lastShape],4			;Case T Shape
	je moveLastPieceTShape
	ret

;Compares which shape the last piece has and move it left
moveLastPieceL:
	cmp word [lastShape],0			;Case B(block) Shape
	je moveLastPieceBShapeL
	cmp word [lastShape],1			;Case I Shape
	je moveLastPieceIShapeL
	cmp word [lastShape],2			;Case L Shape
	je moveLastPieceLShapeL
	cmp word [lastShape],3			;Case Z Shape
	je moveLastPieceZShapeL
	cmp word [lastShape],4			;Case T Shape
	je moveLastPieceTShapeL
	ret

;Compares which shape the last piece has and move it right
moveLastPieceR:
	cmp word [lastShape],0			;Case B(block) Shape
	je moveLastPieceBShapeR
	cmp word [lastShape],1			;Case I Shape
	je moveLastPieceIShapeR
	cmp word [lastShape],2			;Case L Shape
	je moveLastPieceLShapeR
	cmp word [lastShape],3			;Case Z Shape
	je moveLastPieceZShapeR
	cmp word [lastShape],4			;Case T Shape
	je moveLastPieceTShapeR
	ret

;Moves the last piece I to the bottom
;Requires:
;		lastPieceX
;		lastPieceY
moveLastPieceIShape:
	;Delay of .5 s
	cmp word [lastPieceY], 150		;Case lower collision
	je game
	mov word cx, [lastPieceX]		;Loads the x component
	mov word dx, [lastPieceY]		;Loads the y component
	call clearIShape
	mov word cx, [lastPieceX]		;Loads the x component
	mov word dx, [lastPieceY]		;Loads the y component
	add dx, [blockUnit]
	mov word [lastPieceY], dx
	call drawIShape
	ret

;Moves the last piece I to the left
;Requires:
;		lastPieceX
moveLastPieceIShapeL:
	cmp word [lastPieceX], 110		;Case left collision
	je getKey
	call clearIShape
	mov word cx, [lastPieceX]		;Loads the x component
	sub word cx, [blockUnit]		;To the left
	mov word [lastPieceX], cx
	call drawIShape
	ret

;Moves the last piece I to the right
;Requires:
;		lastPieceX
moveLastPieceIShapeR:
	cmp word [lastPieceX], 200		;Case right collision
	je getKey
	call clearIShape
	mov word cx, [lastPieceX]		;Loads the x component
	add word cx, [blockUnit]		;To the right
	mov word [lastPieceX], cx
	call drawIShape
	ret

;Moves the last piece L to the bottom
;Requires:
;		lastPieceX
;		lastPieceY
moveLastPieceLShape:
	;Delay of .5 s
	cmp word [lastPieceY], 160		;Case lower collision
	je game
	mov word cx, [lastPieceX]		;Loads the x component
	mov word dx, [lastPieceY]		;Loads the y component
	call clearLShape
	mov word cx, [lastPieceX]		;Loads the x component
	mov word dx, [lastPieceY]		;Loads the y component
	add dx, [blockUnit]
	mov word [lastPieceY], dx
	call drawLShape
	ret

;Moves the last piece L to the left
;Requires:
;		lastPieceX
moveLastPieceLShapeL:
	cmp word [lastPieceX], 110		;Case left collision
	je getKey
	call clearLShape
	mov word cx, [lastPieceX]		;Loads the x component
	sub word cx, [blockUnit]		;To the left
	mov word [lastPieceX], cx
	call drawLShape
	ret

;Moves the last piece L to the right
;Requires:
;		lastPieceX
moveLastPieceLShapeR:
	cmp word [lastPieceX], 190		;Case right collision
	je getKey
	call clearLShape
	mov word cx, [lastPieceX]		;Loads the x component
	add word cx, [blockUnit]		;To the right
	mov word [lastPieceX], cx
	call drawLShape
	ret

;Moves the last piece Z to the bottom
;Requires:
;		lastPieceX
;		lastPieceY
moveLastPieceZShape:
	;Delay of .5 s
	cmp word [lastPieceY], 170		;Case lower collision
	je game
	mov word cx, [lastPieceX]		;Loads the x component
	mov word dx, [lastPieceY]		;Loads the y component
	call clearZShape
	mov word cx, [lastPieceX]		;Loads the x component
	mov word dx, [lastPieceY]		;Loads the y component
	add dx, [blockUnit]
	mov word [lastPieceY], dx
	call drawZShape
	ret

;Moves the last piece Z to the left
;Requires:
;		lastPieceX
moveLastPieceZShapeL:
	cmp word [lastPieceX], 110		;Case left collision
	je getKey
	call clearZShape
	mov word cx, [lastPieceX]		;Loads the x component
	sub word cx, [blockUnit]		;To the left
	mov word [lastPieceX], cx
	call drawZShape
	ret

;Moves the last piece Z to the right
;Requires:
;		lastPieceX
moveLastPieceZShapeR:
	cmp word [lastPieceX], 180		;Case right collision
	je getKey
	call clearZShape
	mov word cx, [lastPieceX]		;Loads the x component
	add word cx, [blockUnit]		;To the right
	mov word [lastPieceX], cx
	call drawZShape
	ret

;Moves the last piece B(block) to the bottom
;Requires:
;		lastPieceX
;		lastPieceY
moveLastPieceBShape:
	;Delay of .5 s
	cmp word [lastPieceY], 170		;Case lower collision
	je game
	mov word cx, [lastPieceX]		;Loads the x component
	mov word dx, [lastPieceY]		;Loads the y component
	call clearBShape
	mov word cx, [lastPieceX]		;Loads the x component
	mov word dx, [lastPieceY]		;Loads the y component
	add dx, [blockUnit]
	mov word [lastPieceY], dx
	call drawBShape
	ret

;Moves the last piece B(block) to the left
;Requires:
;		lastPieceX
moveLastPieceBShapeL:
	cmp word [lastPieceX], 110		;Case left collision
	je getKey
	call clearBShape
	mov word cx, [lastPieceX]		;Loads the x component
	sub word cx, [blockUnit]		;To the left
	mov word [lastPieceX], cx
	call drawBShape
	ret

;Moves the last piece B(block) to the right
;Requires:
;		lastPieceX
moveLastPieceBShapeR:
	cmp word [lastPieceX], 190		;Case right collision
	je getKey
	call clearBShape
	mov word cx, [lastPieceX]		;Loads the x component
	add word cx, [blockUnit]		;To the right
	mov word [lastPieceX], cx
	call drawBShape
	ret

;Moves the last piece T to the bottom
;Requires:
;		lastPieceX
;		lastPieceY
moveLastPieceTShape:
	;Delay of .5 s
	cmp word [lastPieceY], 160		;Case lower collision
	je game
	mov word cx, [lastPieceX]		;Loads the x component
	mov word dx, [lastPieceY]		;Loads the y component
	call clearTShape
	mov word cx, [lastPieceX]		;Loads the x component
	mov word dx, [lastPieceY]		;Loads the y component
	add dx, [blockUnit]
	mov word [lastPieceY], dx
	call drawTShape
	ret

;Moves the last piece T to the left
;Requires:
;		lastPieceX
moveLastPieceTShapeL:
	cmp word [lastPieceX], 110		;Case left collision
	je getKey
	call clearTShape
	mov word cx, [lastPieceX]		;Loads the x component
	sub word cx, [blockUnit]		;To the left
	mov word [lastPieceX], cx
	call drawTShape
	ret

;Moves the last piece T to the right
;Requires:
;		lastPieceX
moveLastPieceTShapeR:
	cmp word [lastPieceX], 190		;Case right collision
	je getKey
	call clearTShape
	mov word cx, [lastPieceX]		;Loads the x component
	add word cx, [blockUnit]		;To the right
	mov word [lastPieceX], cx
	call drawTShape
	ret

;========;========;=========; Pieces Movement ;=========;========;========;

getKey:
	mov ah, 0x1						;Set ah to 1
	int 0x16						;Check keystroke interrupt
	jz return						;Return if no keystroke
	mov ah, 0x0						;Set ah to 0
	int 0x16						;Get keystroke interrupt
	mov word cx, [lastPieceX]
	mov word dx, [lastPieceY]
	;cmp ah, 0x48					;Jump if up arrow pressed
	;je move_up
	cmp ah, 0x4d					;Jump if right arrow pressed
	je moveLastPieceR
	cmp ah, 0x4b					;Jump if left arrow pressed
	je moveLastPieceL
	ret
	;cmp ah, 0x50					;Jump if down arrow pressed
	;je move_down
;=======;========;==========; Velocity selection ;========;=======;=======;

getMenuKey:
	mov ah, 0x0						;Set ah to 0
	int 0x16						;Check keystroke interrupt
	cmp ah, 0x02					;Jump if key one pressed
	je onePressed
	cmp ah, 0x03					;Jump if key two pressed
	je twoPressed
	cmp ah, 0x04					;Jump if key three pressed
	je threePressed
	ret 
	
;=========;=========;=========; Game main loop ;=========;=========;=========;
game:
	;call check_pac
	call moveLastPiece
	;call get_input	;Check for user inputget_input:
	call getKey
	ret_input:
	cmp word [points], 71	;Game ends 
	je victory
	mov cx, 0x01 	;Delay for 100ms
	mov dx, 0x86a0
	mov ah, 0x86
	int 0x15
	jmp game 		;Loop to itself

;Print a green victory message
victory:
	mov si, v_msg
	mov bl, 2   ;Set green color
	call printMsg
	jmp halt

;Print a red defeat message
defeat:
	mov si, go_msg
	mov bl, 4   	;Set red color
	call printMsg

;Print a message given its color
;Params:
;		bl = color
;		si = message
;		dh = row
;		dl = column
printMsg:
	mov bh, 0  						;Set page 0
	mov cx, 1						;Set number of times

printLoop:
	mov ah, 0x2						;Set cursor position interrupt
	int 10h

	lodsb							;Move si pointer contents to al
	or al, al						;Break if end of string
	jz return

	mov ah, 0xa						;Teletype output interrupt
	int 10h							
	inc dl							;Increment column index
	jmp printLoop					;Loop to itself

;Halt execution
halt:
	mov ah, 0		;Set ah to 0
	int 0x16		;Get keystroke interrupt
	cmp ah, 0x1c	;Restart if enter arrow pressed
	je main
	jmp halt

section .data
	welcomeTitle db 'Tetris EDJ', 0		;Title of the game
	HotKeysTitle db 'HOT KEYS', 0 		;Hot keys section title
	LeftKey db 'Left: <-', 0 			;Hot key left
	RightKey db 'Right: ->', 0 			;Hot key left
	ScoreTitle db 'Score:', 0 			;Score Board
	Score db '0000', 0					;Score
	LevelTitle db 'Level:', 0			;Level Title
	levelNumber1 db '1', 0				;Level	
	levelNumber2 db '2', 0				;Level	
	levelNumber3 db '3', 0				;Level	
	v_msg db 'Winner!', 0
	go_msg	db 'Game Over', 0
	;==============MENU=================================;
	Menu db 'Select a level:', 0		;Select Level
	Level1 db 'Level 1: key 1', 0		;Hot key level 1
	Level2 db 'Level 2: key 2', 0		;Hot key level 2
	Level3 db 'Level 3: key 3', 0		;Hot key level 3

section .bss
	blockUnit resw 1				;Stores the size of one block
	startX resw 1
	startY resw 2
	lastColor resw 1				;Stores the last color (piece)
	lastColorCopy resw 1				;Stores the last color (piece) copy
	lastOrientation resw 1			;Last orientation of the piece
	lastPieceX	resw 1				;X coordinate of the last piece
	lastPieceY 	resw 1				;Y coordinate of the last piece
	lastShape resw 1				;Stores the shape of the last piece
	velocity resw 1					;Stores velocity
	;================MENU================================;
	actualLevel resw 1				;Stores actual level 
	actualSpeed resw 0x0001			;Stores the speed level

	points 		resw 1