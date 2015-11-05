#include "serial_pl011.h"

#define DEBUG_UART_PHYS		0xf8015000
#define DEBUG_UART_VIRT		0xf0000000

	.macro	addruart, rp, rv, tmp
	ldr	\rp, =DEBUG_UART_PHYS
	ldr	\rv, =DEBUG_UART_VIRT
	.endm

	.macro	senduart, rd, rx
	strb	\rd, [\rx, #UART01x_DR]
	.endm

	.macro	waituart, rd, rx
1001:	ldr	\rd, [\rx, #UART01x_FR]
	tst	\rd, #UART01x_FR_TXFF
	bne	1001b
	.endm

	.macro	busyuart, rd, rx
1001:	ldr	\rd, [\rx, #UART01x_FR]
	tst	\rd, #UART01x_FR_BUSY
	bne	1001b
	.endm

	.macro	addruart_current, rx, tmp1, tmp2
	addruart		\rx, \tmp1, \tmp2
	.endm

	.global printascii
	.global printch
	.global	printhex8

printhex8:
	mov	r1, #8
	b	printhex

printhex4:
	mov	r1, #4
	b	printhex

printhex2:
	mov	r1, #2
printhex:
	adr	r2, hexbuf
	add	r3, r2, r1
	mov	r1, #0
	strb	r1, [r3]
1:	and	r1, r0, #15
	mov	r0, r0, lsr #4
	cmp	r1, #10
	addlt	r1, r1, #'0'
	addge	r1, r1, #'a' - 10
	strb	r1, [r3, #-1]!
	teq	r3, r2
	bne	1b
	mov	r0, r2
	b	printascii

hexbuf:	.space 16
	.ltorg

printascii:
	addruart_current r3, r1, r2
	b	2f
1:	waituart r2, r3
	senduart r1, r3
	busyuart r2, r3
	teq	r1, #'\n'
	moveq	r1, #'\r'
	beq	1b
2:	teq	r0, #0
	ldrneb	r1, [r0], #1
	teqne	r1, #0
	bne	1b
	mov	pc, lr

printch:
	addruart_current r3, r1, r2
	mov	r1, r0
	mov	r0, #0
	b	1b
