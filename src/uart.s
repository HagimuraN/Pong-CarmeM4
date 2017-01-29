.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

.section .data.globalvariable
.align
receive_mode: .byte 0x00		//0: receive paddle position of 2nd player
							//1: receive all three coordinate from game master
.align
COORDINATE_ADDRESSES: .space 4*6, 0x00	//All three coordinates is stored in addresses stored here

.align
RECEIVE_BUFFER:		  .space 6*2, 0x00	//All received data are temporarely stored here after interrupt


//Konstanten
//RCC Registers
.set RCC_BASE_ADDR,  0x40023800
.set OFFSET_APB2ENR, 0x44
//UART1 Registers
.set USART1_BASE_ADDR, 0x40011000
.set OFFSET_ADDR_DATA, 0x04
.set OFFSET_ADDR_BAUD, 0x08
.set OFFSET_ADDR_CR1, 0x0C
.set OFFSET_ADDR_CR2, 0x10
.set OFFSET_ADDR_CR3, 0x14
//NVIC_ISER_1 Register for enabling UART1 Interrupt
.set NVIC_ISER_1, 0xE000E104

.set DIV_Manzisse, 91
.set DIV_Frak, 1

.global usart_init
.section .text.init
usart_init:
	//Set receive mode for interrupt routine to decide how to save data into buffer
	LDR r1, =receive_mode
	STR r0, [r1]

	//Enable Clock for UART1
	LDR r0, =(RCC_BASE_ADDR+OFFSET_APB2ENR)
	LDR r1, [r0]
	ORR r1, r1, #0x10		//USART1 clock enable bit
	STR r1, [r0]

	//set baudrate
	LDR r0, =(USART1_BASE_ADDR+OFFSET_ADDR_BAUD) //load register address
	MOV r1, #(DIV_Manzisse*2*4)		//Baudrate of 115200
	ORR r1, r1, #DIV_Frak
	STRH r1, [r0]						//write in baud rate register (0x2D9)

	//Enable RXNE interrupt by setting RXNEIE
	//set TE to enable transmission
	//set RE to enable reception
	LDR r0, =(USART1_BASE_ADDR+OFFSET_ADDR_CR1)
	LDR r1, =0x002C
	STRH r1, [r0]

/*
	//Clock-Enable and enable clock for the last bit.
	LDR r0, =(USART1_BASE_ADDR+OFFSET_ADDR_CR2)
	LDR r1, =0x840
	STR r1, [r0]

	//Enable One-Bit for disabling noise detection
	LDR r0, =(USART1_BASE_ADDR+OFFSET_ADDR_CR3)
	LDR r1, =0x400
	STR r1, [r0]
*/
	//Enable UART by setting UE-bit in Control-Register1
	LDR r0, =(USART1_BASE_ADDR+OFFSET_ADDR_CR1)
	LDR r1, [r0]
	ORR r1, r1, #(1<<13)
	STRH r1, [r0]


	//Enable UART interrupt in NVIC
	LDR     r0, =NVIC_ISER_1
	LDR     r1, [r0]
	ORR     r1, r1, #(1<<5)
	STR     r1, [r0]

	mov 	pc,lr	//jump back to programm

.global uart_send

.section .text.send
uart_send:
	LDR		r6, =receive_mode	//check mode to decide what to send
	LDR		r7, [r6]
	CMP		r7, #1
	BEQ		send_paddle_pos		//go to send baddle pos-routine if its on 2nd player mode

	LDR		r6, =COORDINATE_ADDRESSES
	POP		{r4, r5}
	STMIA	r6,{r0-r5}

	LDR 	r0, =(USART1_BASE_ADDR+OFFSET_ADDR_DATA)	//USART Data Register
	LDR		r3, =(USART1_BASE_ADDR)		//USART Status Register
	LDR		r4, =COORDINATE_ADDRESSES	//Addresses to coordinate-variable
	ADD		r4, #4*6					//store the end of COORDINATE_ADDRESSES in r4
	loop:
		LDR r1, [r6], #4	//Load Data in Addres r6 to r1, then increment r6 by four to get to the next address data
		LDR	r2, [r3]		//clear TC flag
		STRB r1, [r0]		//Write out to UART1
		wait1:		//Wait for first Byte of 16 bit int variable to send (LSB first)
			LDR r2, [r3]
			AND r2, r2, #0x80	//Filter out TXE bit
			CMP r2, #0
			BEQ wait1

		LSR r1, #8		//Shift the Data-Register, so that it can send upper 8-bit of int
		//LDR r1, [r6], #1	//Load Data in Addres r6 to r1, then increment Address r6 by one
		STRB r1, [r0]		//Write out to UART1
		wait2:		//Wait for the sencond Byte of 16 bit int variable to send (MSB)
			LDR r2, [r3]
			AND r2, r2, #0x80	//Filter out TXE bit
			CMP r2, #0
			BEQ wait2


		CMP r6, r4	//loop until all coordinates are written out
		BNE loop

	mov 	pc,lr

send_paddle_pos:	//only send paddle position (2nd player modus)

	LDR 	r1, =(USART1_BASE_ADDR+OFFSET_ADDR_DATA)	//USART Data Register
	LDR		r3, =(USART1_BASE_ADDR)		//USART Status Register
//	LDR		r0, [r0]		//load input data

	LDR	r2, [r3]		//clear TC flag
	STRB r0, [r1]		//Write out to UART1
	wait_1:		//Wait for first Byte of 16 bit int variable to send (LSB first)
		LDR r2, [r3]
		AND r2, r2, #0x80	//Filter out TXE bit
		CMP r2, #0
		BEQ wait_1

	LSR r1, #8		//Shift the Data-Register, so that it can send upper 8-bit of int
//	LDR r1, [r6], #1	//Load Data in Addres r6 to r1, then increment Address r6 by one
	STRB r1, [r0]		//Write out to UART1
	wait_2:		//Wait for the sencond Byte of 16 bit int variable to send (MSB)
		LDR r2, [r3]
		AND r2, r2, #0x80	//Filter out TXE bit
		CMP r2, #0
		BEQ wait_2

	mov 	pc,lr


//Read received Data from Buffer
.global uart_receive
.section .text.receive
uart_receive:
//First read mode to determine what data is stored in buffer
	LDR		r4, =receive_mode
	CMP		r4, #1
	BNE		read_paddle

	//Möglicherweise braucht man den ORR gar nicht, da alles schon eingelesen werden (32-bit mässig)
	//Read coordinates from Buffer
	//LSB are stored in lower address
	LDR		r4, =RECEIVE_BUFFER			//first coordinate x
	LDR		r5, =(RECEIVE_BUFFER+1)
	ORR		r4, r4, r5, LSL #8
	STRH	r4, [r0]

	LDR		r4, =(RECEIVE_BUFFER+2)		//first coordinate y
	LDR		r5, =(RECEIVE_BUFFER+3)
	ORR		r4, r4, r5, LSL #8
	STRH	r4, [r1]

	LDR		r4, =(RECEIVE_BUFFER+4)		//2nd coordinate x
	LDR		r5, =(RECEIVE_BUFFER+5)
	ORR		r4, r4, r5, LSL #8
	STRH	r4, [r2]

	LDR		r4, =(RECEIVE_BUFFER+6)		//2nd coordinate y
	LDR		r5, =(RECEIVE_BUFFER+7)
	ORR		r4, r4, r5, LSL #8
	STRH	r4, [r3]

	POP		{r0, r1}	//last two addresses for variable are stored in stack
	//Reihenfolge richtig?
	LDR		r4, =(RECEIVE_BUFFER+8)
	LDR		r5, =(RECEIVE_BUFFER+9)
	ORR		r4, r4, r5, LSL #8
	STRH	r4, [r0]

	LDR		r4, =(RECEIVE_BUFFER+10)
	LDR		r5, =(RECEIVE_BUFFER+11)
	ORR		r4, r4, r5, LSL #8
	STRH	r4, [r1]

	BX		LR
read_paddle:
	LDR		r4, =RECEIVE_BUFFER			//paddle coordinate
	LDR		r5, =(RECEIVE_BUFFER+1)
	ORR		r4, r4, r5, LSL #8
	STRH	r4, [r0]

	BX		LR


.section	.data.USART1_IRQHandler_data
	buffer_index:	.byte	0x00

//Data will be saved on Buffer on interrupt
.section	.text.USART1_IRQHandler
	.weak	USART1_IRQHandler
	.type	USART1_IRQHandler,	%function
USART1_IRQHandler:
	LDR		r0, =USART1_BASE_ADDR
	LDR		r1, [r0]			//Load status register to check the receive flag
	AND		r1, r1, #(1<<5)
	CMP		r1, #(1<<5)				//Check if the Flag is set
	BNE		exit				//ignore if not

	LDR		r0, =receive_mode	//Check receive modus to check how many Bytes to place on Buffer
	LDR		r1, [r0]
	CMP		r1, #1
	BNE		paddle		//if mode-paddle then jump to paddle-subroutine, else expect coordnate of all three elements

	//receive coordinate
	LDR		r0, =RECEIVE_BUFFER
	LDR		r1, =buffer_index
	LDR		r2, =(USART1_BASE_ADDR+OFFSET_ADDR_DATA)
	LDR		r3, [r2]		//read value in UART-Data-Register

	STRB	r3, [r0, r1]	//Store in Receive buffer with calculated offset
	ADD		r1, r1, #1		//increment offset

	CMP		r1, #(6*2)		//if maximum amount of data has been loaded:
	BNE		exit
	MOV		r1, #0			//reset the offset

	B		exit			//end this routine
paddle:	//receive a single int value
	LDR		r0, =RECEIVE_BUFFER
	LDR		r1, =buffer_index
	LDR		r2, =(USART1_BASE_ADDR+OFFSET_ADDR_DATA)
	LDR		r3, [r2]		//read value in UART-Data-Register

	STRB	r3, [r0, r1]	//Store in Receive buffer with calculated offset
	ADD		r1, r1, #1		//increment offset

	CMP		r1, #(1*2)		//if maximum amount of data has been loaded:
	BNE		exit
	MOV		r1, #0			//reset the offset

	B		exit			//end this routine

exit:
	BX		LR
//comment	.weak		USART1_IRQHandler
//			.thumb_set	USART1_IRQHandler, Default_Handler

