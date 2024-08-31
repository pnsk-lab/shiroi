/* $Id$ */

#include "dri/text.h"

unsigned char basicbuffer[1024 * 30];

void basic(void){
	clear();

	putstr("Shiroi Microcomputer BASIC\r\n");
	putstr("Copyright 2024 by Nishi\r\n");
	cursor();
	int lineind = 0;
	while(1){
		char c = agetch();
		if(c != 0) killcursor();
		if(c == 1){
			putstr("Break\r\n");
			lineind = 0;
		}else if(c == '\n'){
			putstr("\r\n");
			lineind = 0;
		}else if(c == 0x8){
			if(lineind > 0){
				putstr("\x08 \x08");
				lineind--;
			}
		}else if(c != 0){
			putchar(c);
			lineind++;
		}
		cursor();
	}
}
