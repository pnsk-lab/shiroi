/* $Id$ */

#include "dri/text.h"

unsigned char basicbuffer[1024 * 30];

void basic(void){
	clear();

	putstr("Shiroi Microcomputer BASIC\r\n");
	cursor();
	while(1){
		char c = agetch();
		if(c == '\n'){
			killcursor();
			putstr("\r\n");
		}else if(c != 0){
			killcursor();
			putchar(c);
		}
		cursor();
	}
}
