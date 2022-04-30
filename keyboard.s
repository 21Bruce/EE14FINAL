	INCLUDE core_cm4_constants.s		; Load Constant Definitions
	INCLUDE stm32l476xx_constants.s      

	AREA    keyboardArea, CODE, READONLY
	EXPORT	keyboard				; make __main visible to linker
	ENTRY			
				
keyboard	PROC
	PUSH{r1-r12,lr}
	;initialization done outside the function in main.c
	
	MOV r12, #1 ; amount of buttons pressed
restart 
	;restore the row outputs
	LDR r0, =GPIOE_BASE
	LDR r1, [r0, #GPIO_ODR]
	BIC r1, r1, #(0xF<<10) 
    STR r1, [r0, #GPIO_ODR]
	
	MOV r1, #0x02 ;i: starts at 1
	MOV r0, #0x00 ;column pressed output
	MOV r2, #0
	MOV r3, #0
	MOV r4, #0
	MOV r5, #0
	MOV r6, #0
	MOV r7, #0
	MOV r8, #0
	MOV r9, #0

col_loop 
	
	MOV r0, #0
	;call func
	BL column_finder ;call func
	CMP r0, #0  ;if its 0 then its pressed
	LSLNE r1, #1
	MOVEQ r8, r1 ; store the column code IN R8
	MOVEQ r2, #0x07 ; 0111 setup for row search
	
	CMP r1, #0x10
	LSLEQ r1, #1	;SKIP PA4
	
	CMP r1, #0x40 ;start over when passed pa5
	MOVEQ r1, #2
	BEQ col_loop
	
	CMP r0, #0  
	BNE col_loop
	
	BEQ	row_loop
	
	CMP r1, #0x08 
	LSLLT r1, r1, #1  ;get r1 to be 001, 010, then 100
	BLT col_loop
	B restart ;no button pressed, restart search
	;should have column number, not index, in r0. if r0 is 0, no button pressed
	
row_loop
	BL row_finder ;call func 
	MOV r9, r0 ; get the row code IN R9
	;now the row code is in r9 and the column code is in r8. Note that row code is flipped: first tested is r4 and not r1
	MOV r0, #0
	
	;get actual row and column number: will be input+-
	MOV r1, r9 ;store row code in r1
	MOV r0, r8 ;store column code in r0
	
	BL col_row_code_conv ;get col and row number
	
	;retrieve location on keypad
	MOV r3, #4
	SUB r1, r1, #1 ;get row-1
	MUL r3, r3, r1 ;r3 = 4*(row-1)
	ADD r3, r3, r0 ;r3 = 4*(row-1) + col
	SUB r3, r3, #1 ; -1 bc of indexing
	LSL r3, r3, #2
	;retrieve value 
	CMP r0, #5
	SUBEQ r3, #1
	LDR r0,=array
	LDR r0, [r0, r3] ;add offset to get value
	
	;now have the value of the button pressed in r0
	;PUSH{r0} ;push on the stack
	MOV r11, r0
	
	;restore the row outputs
	LDR r0, =GPIOE_BASE
	LDR r1, [r0, #GPIO_ODR]
	BIC r1, r1, #(0xF<<10) 
    STR r1, [r0, #GPIO_ODR]
	;wait until unpress button. LOOP
unpress BL column_finder_modified
	CMP r0, #0  ;=0 if nothing pressed
	BNE unpress ;if r0 = 1, meaning a button is pressed, loop again
	 
	ADD r12, r12, #1
	CMP r12, #2 ;expecting one press
	
    BLT restart
	MOV r0, r11 ;output
	
	POP{r1-r12,pc}
stop 	B 		stop     		; dead loop & program hangs here
	ENDP
  
row_finder PROC
	PUSH{r4-r7, lr}
	;set the row
ini MOV r6, r2 ; put input in r6
	LSL r6, r6, #0xA ;shift by 10 to get bits 10 to 13
	
	LDR r4, = GPIOE_BASE
	LDR r5, [r4, #GPIO_ODR]
	BIC r5, r5, #(0xF << 0xA) ;bit clear bits 10 to 13
	ORR r5, r5, r6
	STR r5, [r4, #GPIO_ODR] ;change output register
	
	;implement delay
	LDR r11, =0  ;30 ms
recount	ADD r11, #1
	CMP r11, #1000
	BNE recount
	
	;check the column
	BL column_finder_modified
	CMP r0, #0
	LSREQ r2, r2, #1    ;if row not right, change row
	ADDEQ r2, r2, #0x08; shift right and add a 1 at the MSB
	
	BEQ ini
	MOVNE r0, r2   ;return the correct in r0
	
	POP{r4-r7, lr}
	BX LR
	ENDP
			
					  ;output: r0
column_finder PROC  ;input: r1, being the column counter
	PUSH{r4-r7, lr}
	LDR r4, = GPIOA_BASE
	LDR r5, [r4, #GPIO_IDR]
	;;MOV r11, r5 ;;test
	AND r0, r5, r1
	
	POP{r4-r7, lr}
	BX LR
	ENDP
  
  
column_finder_modified PROC  
	PUSH{r4-r7, lr}
	
	MOV r7, #0x2E ; 10 1110
	LDR r4, = GPIOA_BASE
	LDR r5, [r4, #GPIO_IDR]
	AND r0, r5, r7 ;MASK to see if all are high
	CMP r0, r7
	MOVEQ r0, #0 ; if they are the same then the column is not sollicited
	MOVNE r0, #1 
	
	POP{r4-r7, lr}
	BX LR
	ENDP
  
  
col_row_code_conv PROC
	PUSH{r4-r7, lr}
	MOV r4, r0  ;inputs
	MOV r5, r1
	
	;columns decoding
	MOV r6, #1 ;counter: starts at column 1
compare	CMP r4, #1	   ;compare it to 0x000001
	LSRNE r4, r4, #1 ;shift right once
	ADDNE r6, r6, #1 ;increment counter
	BNE compare ;shift until the number is equal to 1 
	MOVEQ r0, r6
	SUBEQ r0, #1

	;row decoding
	MOV r6, #1 ;counter
	MOV r7, #1 ;0x0001 reference that we will shift if needed
anding 	AND r1, r7, r5 ; AND the reference and the row code
	 
	CMP r1, #0
	MOVEQ r1, r6 ; return the counter
	LSLNE r7, r7, #1    ;shift the reference
	ADDNE r6, r6, #1
	
	BNE anding ;and again 

	POP{r4-r7, lr}
	BX LR
	ENDP
	  				
	ALIGN			

	AREA    myData, DATA, READWRITE
	ALIGN
array	DCD   1, 2, 3, 0xA, 4, 5, 6, 0xB, 7, 8, 9, 0xC, 0x433, 0, 0x439 ;where non digits are random values. 
												   
	END
