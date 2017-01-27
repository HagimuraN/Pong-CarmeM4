.global Carme_I01_Button_S0

.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

//definition of the subroutines
.text
Carme_I01_Button_S0:
	LDR		r1,=0x6C000400
	LDR		r2,[r1]
	AND		r0, r2, #1<<0
	MOV		pc,lr





