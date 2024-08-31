/* $Id$ */

#include "dri/text.h"

#include "char.h"

#define BUFFER_SIZE (1024 * 16)

unsigned char basicbuffer[BUFFER_SIZE];
char linebuf[1024];

void basic(void){
	clear();

	putstr("Shiroi Microcomputer BASIC\r\n");
	putstr("Copyright 2024 by Nishi\r\n");
	putnum(BUFFER_SIZE);
	putstr(" bytes free\r\n");
	cursor();
	linebuf[0] = 0;
	int lineind = 0;
	while(1){
		char c = agetch();
		if(c != 0) killcursor();
		if(c == 1){
			putstr("Break\r\n");
			lineind = 0;
		}else if(c == '\n'){
			linebuf[lineind] = 0;
			putstr("\r\n");
			lineind = 0;
		}else if(c == 0x8){
			if(lineind > 0){
				putstr("\x08 \x08");
				lineind--;
			}
		}else if(c != 0){
			putchar(c);
			linebuf[lineind++] = c;
		}
		cursor();
	}
}
