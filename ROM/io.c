/* $Id$ */

#include "io.h"

unsigned char inp(unsigned char port) __naked {
__asm
	ld      hl,#2
	add     hl,sp
	ld      c,(hl)
	in      a,(c)
	ld      l,a
	ld      h,#0
	ret
__endasm;
}

void outp(unsigned char port, unsigned char data) __naked {
__asm
	push bc
	ld      hl,#4
	add     hl,sp
	ld      c,(hl)
	inc     hl
	ld      b,(hl)
	out     (c),b
	pop     bc
	ret
__endasm;
}
