/* $Id$ */

#include "debug.h"

#include "../io.h"

short debug_addr;
short debug_data;

void debug_card(int t, int port){
	if(t == 0x23){
		debug_addr = port - 2;
		debug_data = port - 1;
		debug_number(0x0000);
	}
}

void debug_put_number(unsigned char addr, unsigned char n){
	if(debug_addr == -1) return;

	const unsigned char hex[] = {
		0b01110111,
		0b00100100,
		0b01011101,
		0b01101101,
		0b00101110,
		0b01101011,
		0b01111011,
		0b00100111,
		0b01111111,
		0b01101111,
		0b00111111,
		0b01111010,
		0b01010011,
		0b01111100,
		0b01011011,
		0b00011011
	};

	outp(debug_addr, addr);
	outp(debug_data, hex[n]);
}

void debug_number(unsigned short n){
	int i;
	for(i = 0; i < 4; i++){
		debug_put_number(i, (n & 0xf000) >> 12);
		n = n << 4;
	}
}
