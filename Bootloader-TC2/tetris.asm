;************************************************************
;	Instituto Tecnológico de Costa Rica
;	Computer Engineering
;
;	Programmers: Esteban Agüero Pérez
;				 Jeremy Rodriguez Solorzano
;				 Daniela Hernandez Alvarado
;
;	Last update: 12/03/2019
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

	mov word [blockUnit], 10 		;Size of block
	mov word [lastColor], 5			;The last color

	mov al, 0x3						;Set dark gray color
	mov cx, 110						;Set column (x) to 110
	mov dx, 10						;Set row (y) to 1
	call drawBoard

	mov word [startX], 150			;Loading starting position
	mov word [startY], 10

	mov word [points], 0			;Clearing points

	;when its generated check the x and y
	mov word cx, [startX]
	mov word dx, [startY]

	mov word [lastPieceX], cx
	mov word [lastPieceY], dx

	;mov word cx, [lastPieceX]		;draw a piece
	;mov word dx, [lastPieceY]

	call drawIShape

	;mov cx, 140
	;mov dx, 70
	;call drawTShape

	;mov cx, 120
	;mov dx, 90
	;call drawLShape

	jmp game 						;Game main loop

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

;Drawing a large box
draw_large_box:
	pusha			;Push registers onto the stack
	int 0x10		;Draw initial pixel
	mov bx, cx		;Move initial x position to bx
	add bx, 80		;Add 80 to determine the final position of the block
	call drawLineX	;Draw top horizontal line
	sub cx, 80		; Substract 80 to obtain initial value
	add dx, 60		; Add 70 to determine the position of the down horizontal line
	call drawLineX 	; Draw bottom horizontal line
	sub dx, 60		; Substract 70 to obtain initial value
	sub cx, 80		; Substract 80 to obtain initial value
	mov bx, dx		; Move dx to bx
	add bx, 60		; Add 70 to obtain final value
	call drawLineY	;Draw left vertical line
	add cx, 80		; Add 80 to obtain second vertical line initial position
	sub dx, 60		; Substract 70 to obtain initial value
	call drawLineY	;Draw right vertical line
	popa			;Pops registers from the stack
	ret				; Return

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

;Draw a 4x4 dot in the center of a 20x20 grid
draw_dot:
	pusha
	mov al, 0x6		;Set brown color
	add cx, 9		;Set x as cx+9
	add dx, 9		;Set y as dx+9
	int 0x10 		;Draw the x,y pixel
	inc cx			;Set x as cx+10
	int 0x10 		;Draw the x,y pixel
	dec cx			;Set x as cx+9
	inc dx 			;Set y as dx+10
	int 0x10 		;Draw the x,y pixel
	inc cx			;Set x as cx+10
	int 0x10 		;Draw the x,y pixel
	popa
	ret

;Put pellets on a row
fill_dots_x:
	cmp cx, 300		;Compare if currrent x is greater than the desired x
	jg return 		;Returns if true
	call draw_dot	;Draws a dot in the selected 20x20 grid
	add cx, 20		;Adds 32 to cx to get the required dots
	jmp fill_dots_x	;Loops to itself

;Put pellets on a column
fill_dots_y:
	cmp dx, 170		;Compare if currrent y is greater than the desired y
	jg return 		;Returns if true
	call draw_dot	;Draws a dot in the selected 20x20 grid
	add dx, 20		;Adds 45 to dx to get the required dots
	jmp fill_dots_y	;Loops to itself

;Draws a rectangle given its width and height
draw_rectangle:
	mov ah, 0x0c								;Write graphics pixel
	int 0x10										;draws the first pixel
	;draws the top line
	pusha												;saves the registers
	mov word bx, [rectangle_w]	;gets the rectangle width
	add bx, cx									;calculate the x boundary
	call drawLineX						;draws the line
	popa												;restores the registers
	;draws the bottom line
	pusha												;saves the registers
	mov word bx, [rectangle_h] 	;gets the rectangle height
	add dx, bx									;obtains the initial y for bottom line
	int 0x10										;draws the first pixel
	mov word bx, [rectangle_w]	;gets the rectangle width
	add bx, cx									;calculate the x boundary
	call drawLineX						;draws the line
	popa												;resotores the registers
	;draws the left line
	pusha												;saves the registers
	mov word bx, [rectangle_h] 	;gets the rectangle height
	add bx, dx									;obtains the y boundary
	call drawLineY						;draws the line
	popa												;resotores the registers
	;draws the right line
	pusha												;saves the registers
	mov word bx, [rectangle_w]	;gets the rectangle width
	add cx, bx									;get the x for the right line
	mov word bx, [rectangle_h] 	;gets the rectangle height
	add bx, dx									;obtains the y boundary
	call drawLineY						;draws the line
	popa												;resotores the registers
	ret 												;return

;Draws a car
draw_car:
	mov word [car_w], 10        ;saves the car width
	mov word [car_h], 11        ;saves the car height
	mov word bx, [car_w]        ;load the car width
	mov word [rectangle_w], bx  ;saves the width for make a rectangle
	mov word bx,[car_h]         ;loads the car height
	mov word [rectangle_h], bx  ;saves the height for make a rectangle
	mov word cx, [car_x]        ;set the initial x
	mov word dx, [car_y]        ;set the initial y
	call draw_rectangle
	ret 						;return from procedure

;Draws a bus
draw_bus:
	mov word bx, [bus_w]        ;load the bus width
	mov word [rectangle_w], bx  ;saves the width for make a rectangle
	mov word bx,[bus_h]         ;loads the bus height
	mov word [rectangle_h], bx  ;saves the height for make a rectangle
	mov word cx, [bus_x]        ;set the initial x
	mov word dx, [bus_y]        ;set the initial y
	call draw_rectangle
	ret

;Draws a truck
draw_truck:
	mov word bx, [truck_w]      ;load the bus width
	mov word [rectangle_w], bx  ;saves the width for make a rectangle
	mov word bx,[truck_h]       ;loads the bus height
	mov word [rectangle_h], bx  ;saves the height for make a rectangle
	mov word cx, [truck_x]      ;set the initial x
	mov word dx, [truck_y]      ;set the initial y
	call draw_rectangle
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

;=========;=========;=========; Pieces Draws ;=========;=========;=========;

;========;========;=========; Pieces Movement ;=========;========;========;

;Compares which shape the last piece has and move it down
moveLastPiece:
	cmp word [lastShape],1			;Case I Shape
	je moveLastPieceIShape
	ret

;Compares which shape the last piece has and move it left
moveLastPieceL:
	cmp word [lastShape],1			;Case I Shape
	je moveLastPieceIShapeL
	ret

;Compares which shape the last piece has and move it right
moveLastPieceR:
	cmp word [lastShape],1			;Case I Shape
	je moveLastPieceIShapeR
	ret

;Moves the last piece I to the bottom
;Requires:
;		lastPieceX
;		lastPieceY
moveLastPieceIShape:
	;Delay of .5 s
	cmp word [lastPieceY], 150		;Case lower collision
	je game
	pusha
	mov cx, 0x0007
	mov dx, 0xA102
	mov ah, 0x86
	int 0x15
	popa
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
moveLastPieceIShapeL:
	call clearIShape
	mov word cx, [lastPieceX]		;Loads the x component
	sub word cx, [blockUnit]		;To the left
	mov word [lastPieceX], cx
	call drawIShape
	ret

;Moves the last piece I to the right
moveLastPieceIShapeR:
	call clearIShape
	mov word cx, [lastPieceX]		;Loads the x component
	add word cx, [blockUnit]		;To the right
	mov word [lastPieceX], cx
	call drawIShape
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

;Here starts the drawing of pac
;Draws pacman given its color
draw_pac_c:
	mov ah, 0x0c	;Write graphics pixel
	add cx, 1		;X starting point
	add dx, 9		;Y starting point
	mov bx, dx		;Move dx to bx
	mov al, 0		;Set al to 0

;Draws the left half of the character
draw_pac_loop_l:
	cmp al, 6		;Loop for 6 iterations
	je return;je draw_pac_loop_r
	inc cx			;Increment x coordinate
	dec dx 			;Decrement y coordinate
	inc bx 			;Increment vertical line length
	pusha			;Push registers to the stack
	mov al, [pacman_color]		;Set pacman color
	call drawLineY
	popa			;Pop registers from the stack
	inc al			;Increment al
	jmp draw_pac_loop_l		;Loop to itself

;Draws the right half of the character
draw_pac_loop_r:
	cmp al, 0		;Loop for 6 iterations
	je return
	inc cx			;Increment x coordinate
	inc dx 			;Increment y coordinate
	dec bx 			;Decrement vertical line length
	pusha			;Push registers to the stack
	mov al, [pacman_color]		;Set pacman color
	call drawLineY
	popa			;Pop registers from the stack
	dec al			;Decrement al
	jmp draw_pac_loop_r		;Loop to itself

;Till this line

;Checks for user input
get_input:
	mov ah, 0x1		;Set ah to 1
	int 0x16		;Check keystroke interrupt
	jz ret_input	;Return if no keystroke
	mov ah, 0x0		;Set ah to 1
	int 0x16		;Get keystroke interrupt
	mov word cx, [lastPieceX]
	mov word dx, [lastPieceY]
	cmp ah, 0x48	;Jump if up arrow pressed
	je move_up
	cmp ah, 0x4d	;Jump if right arrow pressed
	je move_right
	cmp ah, 0x4b	;Jump if left arrow pressed
	je move_left
	cmp ah, 0x50	;Jump if down arrow pressed
	je move_down
	jmp ret_input

;Actions taken when the up key is pressed
move_up:
	cmp dx, 10		;Do not move if on top border
	je get_input
	pusha			;Push registers to the stack
	add cx, 10		;Center the x axis for collision detection
	call check_col	;Check if collision course
	sub dx, 10		;Look 10 pixels ahead for pellets
	call check_points
	popa			;Pop registers from the stack
	call clear_pac	;Clear the last pacman position
	sub dx, 20		;Update the lastPieceY value in memory
	mov word [lastPieceY], dx
	call draw_pac 	;Draw pacman in its new position
	jmp ret_input	;Return to main loop

;Actions taken when the down key is pressed
move_down:
	cmp dx, 170		;Do not move if on bottom border
	je get_input
	pusha			;Push registers to the stack
	add cx, 10		;Center the x axis for collision detection
	add dx, 20		;Look 20 pixels ahead for collisions
	call check_col
	add dx, 10		;Look 30 pixels ahead for pellets
	call check_points
	popa			;Pop registers from the stack
	call clear_pac	;Clear the last pacman position
	add dx, 20		;Update the lastPieceY value in memory
	mov word [lastPieceY], dx
	call draw_pac 	;Draw pacman in its new position
	jmp ret_input	;Return to main loop

;Actions taken when the left key is pressed
move_left:
	cmp cx, 0		;Do not move if on left border
	je get_input
	pusha			;Push registers to the stack
	add dx, 10		;Center the y axis for collision detection
	call check_col 	;Check if collision course
	sub cx, 10		;Look 10 pixels ahead for pellets
	call check_points
	popa			;Pop registers from the stack
	call clear_pac	;Clear the last pacman position
	sub cx, 20		;Update the lastPieceY value in memory
	mov word [lastPieceX], cx
	call draw_pac 	;Draw pacman in its new position
	jmp ret_input	;Return to main loop

;Actions taken when the right key is pressed
move_right:
	cmp cx, 300		;Do not move if on right border
	je get_input
	pusha			;Push registers to the stack
	add cx, 20		;Look 20 pixels ahead for collisions
	add dx, 10		;Center the y axis for collision detection
	call check_col
	add cx, 9		;Look 9 pixels ahead for pellets
	call check_points
	popa			;Pop registers from the stack
	call clear_pac	;Clear the last pacman position
	add cx, 20		;Update the lastPieceY value in memory
	mov word [lastPieceX], cx
	call draw_pac 	;Draw pacman in its new position
	jmp ret_input	;Return to main loop

;Draws a yellow pacman
draw_pac:
	pusha			;Push registers from the stack
	mov byte [pacman_color], 0xe	;Setting yellow color for pacman
	call draw_pac_c
	popa			;Pop registers from the stack
	ret

;Clears pacman (black color)
clear_pac:
	pusha			;Push registers from the stack
	mov byte [pacman_color], 0x0 	;Setting black color for pacman
	call draw_pac_c
	popa			;Pop registers from the stack
	ret

;Check for collisions on a given coordinate
check_col:
	mov ah, 0x0d	;Get graphics pixel video mode
	mov bh, 0x0 	;Page 0
	int 0x10 		;BIOS Video interrupt
	cmp al, 0x8 	;If pixel is gray, collision
	je get_input
	ret

;Check for pellets on a given coordinate
check_points:
	mov ah, 0x0d	;Get graphics pixel video mode
	mov bh, 0x0 	;Page 0
	int 0x10  		;BIOS Video interrupt
	cmp al, 0x6 	;If pixel is brown, collision
	je inc_points	;Increase points
	ret

;Increases points and draws a progress bar
inc_points:
	pusha			;Push registers from the stack
	mov ah, 0x0c	;Drawing a "progress bar"
	mov cx, [points]
	mov dx, 0
	call draw_dot
	popa			;Push registers from the stack
	inc word [points]	;Increment the points counter
	ret

;Check if a given pacman pixel has been cleared
pac_col:
	mov ah, 0x0d	;Get graphics pixel video mode
	mov bh, 0x0 	;Page 0
	int 0x10  		;BIOS Video interrupt
	cmp al, 0xe 	;If pixel is not yellow, collision
	jne defeat
	ret

;Check all pacman sides for a collision
check_pac:
	mov word cx, [lastPieceX]	;Get current pacman coordinates
	mov word dx, [lastPieceY]
	add cx, 5		;Add an offset for graphics comparison
	add dx, 9
	call pac_col 	;Check for collisions
	add cx, 5		;Add an offset for graphics comparison
	add dx, 5
	call pac_col 	;Check for collisions
	sub dx, 9		;Add an offset for graphics comparison
	call pac_col 	;Check for collisions
	add cx, 5		;Add an offset for graphics comparison
	add dx, 5
	call pac_col 	;Check for collisions
	ret

move_car:
	;validate the horizontal vector
	mov word bx, [car_vx]	;loads the horizontal vector
	cmp bx, 0
	jg move_car_right
	jl move_car_left
	;validate the vertical vector
	mov word bx, [car_vy] ;loads the vertical vector
	cmp bx, 0
	jg move_car_down
	jl move_car_up

move_car_right:
  mov word cx, [car_x]	;loads the x component
	mov word dx, [car_y]	;loads the y component
	cmp cx, 305						;compare with the x boundary
	jge car_turn_u_d_desc	;decides if go up or down
	cmp cx, 105
	je car_random_u_d_r
	cmp cx, 205
	je car_random_u_d_r
	car_right_continue:
	;erase the car
	pusha
	mov al, 0x00    			;set the black color for the car
	call draw_car
	popa
	;move car towards right
	add cx, 20
	mov word [car_x], cx
	mov al, 0x04					;set red color
	pusha
	call draw_car
	popa
	jmp move_bus
	;jmp get_input
car_random_u_d_r:
	mov word [divisor], 2;
	call random
	mov word bx, [random_n]
	cmp bx, 1
	je car_random_u_d_desc
	jmp car_right_continue

move_car_left:
	mov word cx, [car_x]	;loads the x component
	mov word dx, [car_y]	;loads the y component
	cmp cx, 5							;compare with the x boundary
	jle car_turn_u_d_desc	;decides if go up or down
	cmp cx, 105
	je car_random_u_d_l
	cmp cx, 205
	je car_random_u_d_l
	car_left_continue:
	;erase the car
	pusha
	mov al, 0x00    			;set the black color for the car
	call draw_car
	popa
	;move the car towards left
	sub cx, 20
	mov word [car_x], cx
	mov al, 0x04    			;set the red color
	pusha
	call draw_car
	popa
	jmp move_bus
	;jmp get_input

car_random_u_d_l:
	mov word [divisor], 2;
	call random
	mov word bx, [random_n]
	cmp bx, 1
	je car_random_u_d_desc
	jmp car_left_continue

car_random_u_d_desc:
	cmp dx, 14
	je car_turn_d					;turns down
	cmp dx, 174
	je car_turn_u
	jmp car_turn_u_d_rand

car_turn_u_d_desc:
	cmp dx, 14
	je car_turn_d	;turns the car down
	cmp dx, 174
	je car_turn_u	;turn the car up
	jmp car_turn_u_d_rand

car_turn_u_d_rand:
	mov word [divisor], 2;
	call random
	mov word bx, [random_n]
	cmp bx, 0
	je car_turn_u
	cmp bx, 1
	je car_turn_d

car_turn_d:
	;cancels the x movement
	mov bx, 0
	mov word [car_vx], bx
	;turns the car down
	mov bx, 1
	mov word [car_vy], bx
	;move to the new direction
	jmp move_car

car_turn_u:
	;cancels the x movement
	mov bx, 0
	mov word [car_vx], bx
	;turns the car up
	mov bx, -1
	mov word [car_vy], bx
	;move to the new direction
	jmp move_car

move_car_down:
	mov word cx, [car_x]	;loads the x component
	mov word dx, [car_y]	;loads the y component
	cmp dx, 174
	jge car_turn_r_l_desc
	cmp dx, 94
	je car_random_l_r_d
	car_down_continue:
	;erase the car
	pusha
	mov al, 0x00    			;set the black color for the car
	call draw_car
	popa
	;move car down
	add dx, 20
	mov word [car_y], dx
	mov al, 0x04					;set red color
	pusha
	call draw_car
	popa
	jmp move_bus
	;jmp get_input

car_random_l_r_d:
	mov word [divisor], 2;
	call random
	mov word bx, [random_n]
	cmp bx, 1
	je car_random_r_l_desc
	jmp car_down_continue

move_car_up:
	mov word cx, [car_x]	;loads the x component
	mov word dx, [car_y]	;loads the y component
	cmp dx, 14
	jle car_turn_r_l_desc
	cmp dx, 94
	je car_random_l_r_u
	car_up_continue:
	;erase the car
	pusha
	mov al, 0x00    			;set the black color for the car
	call draw_car
	popa
	;move car down
	sub dx, 20
	mov word [car_y], dx
	mov al, 0x04					;set red color
	pusha
	call draw_car
	popa
	jmp move_bus
	;jmp get_input

car_random_l_r_u:
	mov word [divisor], 2;
	call random
	mov word bx, [random_n]
	cmp bx, 1
	je car_random_r_l_desc
	jmp car_up_continue

;decides if turn right or left
car_random_r_l_desc:
	cmp cx, 5
	je car_turn_r
	cmp cx, 305
	je car_turn_l
	jmp car_turn_r_l_rand

;decides if turn right or left
car_turn_r_l_desc:
	cmp cx, 5
	je car_turn_r
	cmp cx, 305
	je car_turn_l

car_turn_r_l_rand:
	mov word [divisor], 2;
	call random
	mov word bx, [random_n]
	cmp bx, 0
	je car_turn_r
	cmp bx, 1
	je car_turn_l

car_turn_r:
	;cancels the vertical movement
	mov bx, 0
	mov word [car_vy], bx
	;turn the car to the right
	mov bx, 1
	mov word [car_vx], bx
	;move to the new direction
	jmp move_car

car_turn_l:
	;cancels the vertical movement
	mov bx, 0
	mov word [car_vy], bx
	;turn the car to the right
	mov bx, -1
	mov word [car_vx], bx
	;move to the new direction
	jmp move_car


move_bus:
	;validate the horizontal vector
	mov word bx, [bus_vx]	;loads the horizontal vector
	cmp bx, 0
	jg move_bus_right
	jl move_bus_left
	;validate the vertical vector
	mov word bx, [bus_vy] ;loads the vertical vector
	cmp bx, 0
	jg move_bus_down
	jl move_bus_up

move_bus_right:
  mov word cx, [bus_x]	;loads the x component
	mov word dx, [bus_y]	;loads the y component
	cmp cx, 285						;compare with the x boundary
	jge bus_turn_u_d_desc_r	;decides if go up or down
	cmp cx, 85
	je bus_random_u_d_r
	cmp cx, 185
	je bus_random_u_d_r
	bus_right_continue:
	;erase the bus
	pusha
	mov al, 0x00    			;set the black color for the bus
	call draw_bus
	popa
	;move bus towards right
	add cx, 20
	mov word [bus_x], cx
	mov al, 0x0C					;set color for the bus
	pusha
	call draw_bus
	popa
	jmp move_truck
	;jmp get_input
bus_random_u_d_r:
	mov word [divisor], 2;
	call random
	mov word bx, [random_n]
	cmp bx, 1
	je bus_random_u_d_desc_r
	jmp bus_right_continue

bus_random_u_d_desc_r:
	;erase the bus
	pusha
	mov al, 0x00    			;set the black color for the bus
	call draw_bus
	popa
	;offset
	add cx, 20
	mov word [bus_x], cx
	jmp bus_random_u_d_desc

bus_turn_u_d_desc_r:
	;erase the bus
	pusha
	mov al, 0x00    			;set the black color for the bus
	call draw_bus
	popa
	;offset
	add cx, 20
	mov word [bus_x], cx
	jmp bus_turn_u_d_desc

move_bus_left:
	mov word cx, [bus_x]	;loads the x component
	mov word dx, [bus_y]	;loads the y component
	cmp cx, 5							;compare with the x boundary
	jle bus_turn_u_d_desc	;decides if go up or down
	cmp cx, 105
	je bus_random_u_d_l
	cmp cx, 205
	je bus_random_u_d_l
	bus_left_continue:
	;erase the bus
	pusha
	mov al, 0x00    			;set the black color for the bus
	call draw_bus
	popa
	;move the bus towards left
	sub cx, 20
	mov word [bus_x], cx
	mov al, 0x0C					;set color for the bus
	pusha
	call draw_bus
	popa
	jmp move_truck
	;jmp get_input

bus_random_u_d_l:
	mov word [divisor], 2;
	call random
	mov word bx, [random_n]
	cmp bx, 1
	je bus_random_u_d_desc
	jmp bus_left_continue

bus_random_u_d_desc:
	;erase the bus
	pusha
	mov al, 0x00    			;set the black color for the bus
	call draw_bus
	popa
	cmp dx, 14
	je bus_turn_d					;turns down
	cmp dx, 174
	je bus_turn_u
	jmp bus_turn_u_d_rand

bus_turn_u_d_desc:
	;erase the bus
	pusha
	mov al, 0x00    			;set the black color for the bus
	call draw_bus
	popa
	cmp dx, 14
	je bus_turn_d	;turns the bus down
	cmp dx, 174
	je bus_turn_u	;turn the bus up
	jmp bus_turn_u_d_rand

bus_turn_u_d_rand:
	mov word [divisor], 2;
	call random
	mov word bx, [random_n]
	cmp bx, 0
	je bus_turn_u
	cmp bx, 1
	je bus_turn_d

bus_turn_d:
	;cancels the x movement
	mov bx, 0
	mov word [bus_vx], bx
	;turns the bus down
	mov bx, 1
	mov word [bus_vy], bx
	;change orientation
	mov word [bus_w], 11
	mov word [bus_h], 30
	;move to the new direction
	jmp move_bus

bus_turn_u:
	;cancels the x movement
	mov bx, 0
	mov word [bus_vx], bx
	;turns the bus up
	mov bx, -1
	mov word [bus_vy], bx
	;change orientation
	mov word [bus_w], 11
	mov word [bus_h], 30
	;move to the new direction
	jmp move_bus


move_bus_down:
	mov word cx, [bus_x]	;loads the x component
	mov word dx, [bus_y]	;loads the y component
	cmp dx, 154
	jge bus_turn_r_l_desc_d
	cmp dx, 74
	je bus_random_l_r_d

bus_down_continue:
	;erase the bus
	pusha
	mov al, 0x00    			;set the black color for the bus
	call draw_bus
	popa
	;move bus down
	add dx, 20
	mov word [bus_y], dx
	mov al, 0x0C
	pusha
	call draw_bus
	popa
	jmp move_truck
	;jmp get_input

bus_random_l_r_d:
	mov word [divisor], 2;
	call random
	mov word bx, [random_n]
	cmp bx, 1
	je bus_random_r_l_desc_d
	jmp bus_down_continue

bus_random_r_l_desc_d:
	;erase the bus
	pusha
	mov al, 0x00    			;set the black color for the bus
	call draw_bus
	popa
	;offset
	add dx, 20
	mov word [bus_y], dx
	jmp bus_random_r_l_desc

bus_turn_r_l_desc_d:
	;erase the bus
	pusha
	mov al, 0x00    			;set the black color for the bus
	call draw_bus
	popa
	;offset
	add dx, 20
	mov word [bus_y], dx
	jmp bus_turn_r_l_desc

move_bus_up:
	mov word cx, [bus_x]	;loads the x component
	mov word dx, [bus_y]	;loads the y component
	cmp dx, 14
	jle bus_turn_r_l_desc
	cmp dx, 94
	je bus_random_l_r_u

bus_up_continue:
	;erase the bus
	pusha
	mov al, 0x00    			;set the black color for the bus
	call draw_bus
	popa
	;move bus down
	sub dx, 20
	mov word [bus_y], dx
	mov al, 0x0C
	pusha
	call draw_bus
	popa
	jmp move_truck

;jmp get_input
bus_random_l_r_u:
	mov word [divisor], 2;
	call random
	mov word bx, [random_n]
	cmp bx, 1
	je bus_random_r_l_desc
	jmp bus_up_continue

;decides if turn right or left
bus_random_r_l_desc:
	;erase the bus
	pusha
	mov al, 0x00    			;set the black color for the bus
	call draw_bus
	popa
	cmp cx, 5
	je bus_turn_r
	cmp cx, 305
	je bus_turn_l
	jmp bus_turn_r_l_rand

;decides if turn right or left
bus_turn_r_l_desc:
	;erase the bus
	pusha
	mov al, 0x00    			;set the black color for the bus
	call draw_bus
	popa
	cmp cx, 5
	je bus_turn_r
	cmp cx, 305
	je bus_turn_l

bus_turn_r_l_rand:
	mov word [divisor], 2;
	call random
	mov word bx, [random_n]
	cmp bx, 0
	je bus_turn_r
	cmp bx, 1
	je bus_turn_l

bus_turn_r:
	;cancels the vertical movement
	mov bx, 0
	mov word [bus_vy], bx
	;turn the bus to the right
	mov bx, 1
	mov word [bus_vx], bx
	;change orientation
	mov word [bus_w], 30
	mov word [bus_h], 11
	;move to the new direction
	jmp move_bus

bus_turn_l:
	;cancels the vertical movement
	mov bx, 0
	mov word [bus_vy], bx
	;turn the bus to the right
	mov bx, -1
	mov word [bus_vx], bx
	;change orientation
	mov word [bus_w], 30
	mov word [bus_h], 11
	;move to the new direction
	jmp move_bus


move_truck:
	;validate the horizontal vector
	mov word bx, [truck_vx]	;loads the horizontal vector
	cmp bx, 0
	jg move_truck_right
	jl move_truck_left
	;validate the vertical vector
	mov word bx, [truck_vy] ;loads the vertical vector
	cmp bx, 0
	jg move_truck_down
	jl move_truck_up

move_truck_right:
	mov word cx, [truck_x]	;loads the x component
	mov word dx, [truck_y]	;loads the y component
	cmp cx, 265						;compare with the x boundary
	jge truck_turn_u_d_desc_r	;decides if go up or down
	cmp cx, 65
	je truck_random_u_d_r
	cmp cx, 165
	je truck_random_u_d_r

truck_right_continue:
	;erase the truck
	pusha
	mov al, 0x00    			;set the black color for the truck
	call draw_truck
	popa
	;move truck towards right
	add cx, 20
	mov word [truck_x], cx
	mov al, 0x02  				;set color for the truck
	pusha
	call draw_truck
	popa
	jmp get_input

truck_random_u_d_r:
	mov word [divisor], 2;
	call random
	mov word bx, [random_n]
	cmp bx, 1
	je truck_random_u_d_desc_r
	jmp truck_right_continue

truck_random_u_d_desc_r:
	;erase the truck
	pusha
	mov al, 0x00    			;set the black color for the truck
	call draw_truck
	popa
	;offset
	add cx, 40
	mov word [truck_x], cx
	jmp truck_random_u_d_desc

truck_turn_u_d_desc_r:
	;erase the truck
	pusha
	mov al, 0x00    			;set the black color for the truck
	call draw_truck
	popa
	;offset
	add cx, 40
	mov word [truck_x], cx
	jmp truck_turn_u_d_desc

move_truck_left:
	mov word cx, [truck_x]	;loads the x component
	jmp ret_input
	mov word dx, [truck_y]	;loads the y component
	cmp cx, 5							;compare with the x boundary
	jle truck_turn_u_d_desc	;decides if go up or down
	cmp cx, 105
	je truck_random_u_d_l
	cmp cx, 205
	je truck_random_u_d_l

truck_left_continue:
	;erase the truck
	pusha
	mov al, 0x00    			;set the black color for the truck
	call draw_truck
	popa
	;move the truck towards left
	sub cx, 20
	mov word [truck_x], cx
	mov al, 0x02					;set color for the truck
	pusha
	call draw_truck
	popa
	jmp get_input

truck_random_u_d_l:
	;random
	mov word [divisor], 2;
	call random
	mov word bx, [random_n]
	cmp bx, 1
	je truck_random_u_d_desc
	jmp truck_left_continue

truck_random_u_d_desc:
	;erase the truck
	pusha
	mov al, 0x00    			;set the black color for the truck
	call draw_truck
	popa
	;random
	cmp dx, 14
	je truck_turn_d					;turns down
	cmp dx, 174
	je truck_turn_u
	jmp truck_turn_u_d_rand

truck_turn_u_d_desc:
	;erase the truck
	pusha
	mov al, 0x00    			;set the black color for the truck
	call draw_truck
	popa
	cmp dx, 14
	je truck_turn_d	;turns the truck down
	cmp dx, 174
	je truck_turn_u	;turn the truck up
	jmp truck_turn_u_d_rand

truck_turn_u_d_rand:
	mov word [divisor], 2;
	call random
	mov word bx, [random_n]
	cmp bx, 0
	je truck_turn_u
	cmp bx, 1
	je truck_turn_d

truck_turn_d:
	;cancels the x movement
	mov bx, 0
	mov word [truck_vx], bx
	;turns the truck down
	mov bx, 1
	mov word [truck_vy], bx
	;change orientation
	mov word [truck_w], 11
	mov word [truck_h], 50
	;move to the new direction
	jmp move_truck

truck_turn_u:
	;cancels the x movement
	mov bx, 0
	mov word [truck_vx], bx
	;turns the truck up
	mov bx, -1
	mov word [truck_vy], bx
	;change orientation
	mov word [truck_w], 11
	mov word [truck_h], 50
	;se acomoda y
	sub dx, 20;
	mov [truck_y], dx
	;move to the new direction
	jmp move_truck


move_truck_down:
	mov word cx, [truck_x]	;loads the x component
	mov word dx, [truck_y]	;loads the y component
	cmp dx, 134
	jge truck_turn_r_l_desc_d
	cmp dx, 54
	je truck_random_l_r_d

truck_down_continue:
	;erase the truck
	pusha
	mov al, 0x00    			;set the black color for the truck
	call draw_truck
	popa
	;move truck down
	add dx, 20
	mov word [truck_y], dx
	mov al, 0x02
	pusha
	call draw_truck
	popa
	jmp get_input

truck_random_l_r_d:
	mov word [divisor], 2;
	call random
	mov word bx, [random_n]
	cmp bx, 1
	je truck_random_r_l_desc_d
	jmp truck_down_continue

truck_random_r_l_desc_d:
	;erase the truck
	pusha
	mov al, 0x00    			;set the black color for the truck
	call draw_truck
	popa
	;offset
	add dx, 40
	mov word [truck_y], dx
	jmp truck_random_r_l_desc

truck_turn_r_l_desc_d:
	;erase the truck
	pusha
	mov al, 0x00    			;set the black color for the truck
	call draw_truck
	popa
	;offset
	add dx, 40
	mov word [truck_y], dx
	jmp truck_turn_r_l_desc

move_truck_up:
	mov word cx, [truck_x]	;loads the x component
	mov word dx, [truck_y]	;loads the y component
	cmp dx, 14
	jle truck_turn_r_l_desc
	cmp dx, 94
	je truck_random_l_r_u
truck_up_continue:
	;erase the truck
	pusha
	mov al, 0x00    			;set the black color for the truck
	call draw_truck
	popa
	;move truck down
	sub dx, 20
	mov word [truck_y], dx
	mov al, 0x02
	pusha
	call draw_truck
	popa
	jmp get_input

truck_random_l_r_u:
	mov word [divisor], 2;
	call random
	mov word bx, [random_n]
	cmp bx, 1
	je truck_random_r_l_desc
	jmp truck_up_continue

;decides if turn right or left
truck_random_r_l_desc:
	;erase the truck
	pusha
	mov al, 0x00    			;set the black color for the truck
	call draw_truck
	popa
	cmp cx, 5
	je truck_turn_r
	cmp cx, 305
	je truck_turn_l
	jmp truck_turn_r_l_rand

;decides if turn right or left
truck_turn_r_l_desc:
	;erase the truck
	pusha
	mov al, 0x00    			;set the black color for the truck
	call draw_truck
	popa
	cmp cx, 5
	je truck_turn_r
	cmp cx, 305
	je truck_turn_l

truck_turn_r_l_rand:
	mov word [divisor], 2;
	call random
	mov word bx, [random_n]
	cmp bx, 0
	je truck_turn_r
	cmp bx, 1
	je truck_turn_l

truck_turn_r:
	;cancels the vertical movement
	mov bx, 0
	mov word [truck_vy], bx
	;turn the truck to the right
	mov bx, 1
	mov word [truck_vx], bx
	;change orientation
	mov word [truck_w], 50
	mov word [truck_h], 11
	;move to the new direction
	jmp move_truck

truck_turn_l:
	;cancels the vertical movement
	mov bx, 0
	mov word [truck_vy], bx
	;turn the truck to the right
	mov bx, -1
	mov word [truck_vx], bx
	;change orientation
	mov word [truck_w], 50
	mov word [truck_h], 11
	;adjust x
	sub cx, 20;
	mov [truck_x], cx
	;move to the new direction
	jmp move_truck


move_enemies:
	;espera 0.5 segundos
	pusha
	mov cx, 0x0007
	mov dx, 0xA102
	mov ah, 0x86
	int 0x15
	popa
	call move_car

random:
	pusha
  mov ah, 0x0
  int 0x1a
  mov  ax, dx
  xor  dx, dx
  mov word  cx, [divisor] 	;gets the divisor
  div  cx
	mov word [random_n], dx	;random number
	popa
  ret

;=========;=========;=========; Game main loop ;=========;=========;=========;
game:
	;call check_pac
	call moveLastPiece
	;call get_input	;Check for user inputget_input:
	call getKey
	ret_input:
	cmp word [points], 71	;Game ends when player eats all of the pellets
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
	welcomeTitle db 'Tetris EDJ', 0	;Title of the game
	HotKeysTitle db 'HOT KEYS', 0 ;Hot keys section title
	LeftKey db 'Left: <-', 0 ;Hot key left
	RightKey db 'Right: ->', 0 ;Hot key left
	v_msg db 'Tetris In progress!', 0
	go_msg	db 'Game Over', 0

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

	pacman_color resb 1
	points 		resw 1
	rectangle_w resw 1
	rectangle_h resw 1
	car_x resw 1
	car_y resw 1
	bus_x resw 1
	bus_y resw 1
	truck_x resw 1
	truck_y resw 1
	car_w resw 1
	car_h resw 1
	bus_w resw 1
	bus_h resw 1
	truck_w resw 1
	truck_h resw 1
	car_vx resw 1
	car_vy resw 1
	bus_vx resw 1
	bus_vy resw 1
	truck_vx resw 1
	truck_vy resw 1
	divisor resw 1
	random_n resw 1