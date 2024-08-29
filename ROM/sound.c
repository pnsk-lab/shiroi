/* $Id$ */

#include "sound.h"

#include "io.h"

short psg_addr;
short psg_data;

void beep(void){
	_beep(1L * 1024);
}

void _beep(unsigned long howlong){
	if(psg_addr == -1) return;
	unsigned long i;

	outp(psg_addr, 8);
	outp(psg_data, 0x0f);

	outp(psg_addr, 0);
	outp(psg_data, 0xb0);

	outp(psg_addr, 1);
	outp(psg_data, 0x0);

	outp(psg_addr, 7);
	outp(psg_data, 0x3e);

	for(i = 0; i < howlong; i++);

	outp(psg_addr, 7);
	outp(psg_data, 0x3f);
}

void sound_card(int t, int port){
	if(t == 0x11){
		psg_addr = port - 2;
		psg_data = port - 1;
	}
}
