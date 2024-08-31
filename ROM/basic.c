/* $Id$ */

#include "dri/text.h"

unsigned char basicbuffer[1024 * 30];

void basic(void){
	clear();

	putstr("Shiroi Microcomputer BASIC\r\n");
	putstr("Copyright 2024 by Nishi\r\n");
	cursor();
	while(1){
		char c = agetch();
		if(c != 0) killcursor();
		if(c == 1){
			putstr("Break\r\n");
		}else if(c == '\n'){
			putstr("\r\n");
		}else if(c != 0){
			putchar(c);
		}
		cursor();
	}
}
