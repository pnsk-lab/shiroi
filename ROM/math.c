/* $Id$ */

#include "math.h"

#include "io.h"

#define AM_SR       0x80 /* service request on completion */
#define AM_SINGLE   0x60 /* 16 bit integer */
#define AM_DOUBLE   0x20 /* 32 bit integer */
#define AM_FIXED    0x20 /* fixed point */
#define AM_FLOAT    0x00 /* 32 bit float */

#define AM_NOP      0x00 /* no operation */
#define AM_SQRT     0x01 /* square root */
#define AM_SIN      0x02 /* sine */
#define AM_COS      0x03 /* cosine */
#define AM_TAN      0x04 /* tangent */
#define AM_ASIN     0x05 /* inverse sine */
#define AM_ACOS     0x06 /* inverse cosine */
#define AM_ATAN     0x07 /* inverse tangent */
#define AM_LOG      0x08 /* common logarithm (base 10) */
#define AM_LN       0x09 /* natural logairth (base e) */
#define AM_EXP      0x0a /* exponential (e^x) */
#define AM_PWR      0x0b /* power nos^tos */
#define AM_ADD      0x0c /* add */
#define AM_SUB      0x0d /* subtract nos-tos */
#define AM_MUL      0x0e /* multiply, lower half */
#define AM_DIV      0x0f /* divide nos/tos */
#define AM_FADD     0x10 /* floating add */
#define AM_FSUB     0x11 /* floating subtract */
#define AM_FMUL     0x12 /* floating multiply */
#define AM_FDIV     0x13 /* floating divide */
#define AM_CHS      0x14 /* change sign */
#define AM_CHSF     0x15 /* floating change sign */ 
#define AM_MUU      0x16 /* multiply, upper half */
#define AM_PTO      0x17 /* push tos to nos (copy) */
#define AM_POP      0x18 /* pop */
#define AM_XCH      0x19 /* exchange tos and nos */
#define AM_PUPI     0x1a /* push pi */
/*                  0x1b */
#define AM_FLTD     0x1c /* 32 bit to float */
#define AM_FLTS     0x1d /* 16 bit to float */
#define AM_FIXD     0x1e /* float to 32 bit */
#define AM_FIXS     0x1f /* float to 16 bit */

#define AM_BUSY     0x80 /* chip is busy */
#define AM_SIGN     0x40 /* tos negative */
#define AM_ZERO     0x20 /* tos zero */
#define AM_ERR_MASK 0x1E /* mask for errors */
#define AM_CARRY    0x01 /* carry/borrow from most significant bit */

short fpu_stack;
short fpu_command;

long modl(long a, long b){
	return a - mull(b, divl(a, b));
}

long divl(long a, long b){
	int i;
	for(i = 0; i < 4; i++){
		outp(fpu_stack, a & 0xff);
		a = a >> 8;
	}
	for(i = 0; i < 4; i++){
		outp(fpu_stack, b & 0xff);
		b = b >> 8;
	}
	outp(fpu_command, AM_DOUBLE | AM_DIV);
	while(!(inp(fpu_command) & 1));
	unsigned long r = 0;
	for(i = 0; i < 4; i++){
		r = r << 8;
		r |= inp(fpu_stack);
	}
	return r;
}

long mull(long a, long b){
	int i;
	for(i = 0; i < 4; i++){
		outp(fpu_stack, a & 0xff);
		a = a >> 8;
	}
	for(i = 0; i < 4; i++){
		outp(fpu_stack, b & 0xff);
		b = b >> 8;
	}
	outp(fpu_command, AM_DOUBLE | AM_MUL);
	while(!(inp(fpu_command) & 1));
	long r = 0;
	for(i = 0; i < 4; i++){
		r = r << 8;
		r |= inp(fpu_stack);
	}
	return r;
}

void math_card(int t, int port){
	if(t == 0x21){
		fpu_stack = port - 2;
		fpu_command = port - 1;
	}
}

int muli(int a, int b){
	int n = 0;
	int i;
	if(b > 0){
		for(i = 0; i < b; i++) n += a;
	}else if(b < 0){
		for(i = 0; i > b; i--) n -= a;
	}
	return n;
}
