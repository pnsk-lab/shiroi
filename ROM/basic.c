/* $Id$ */

#include "text.h"

unsigned char basicbuffer[1024 * 30];

void basic(void){
	clear();

	putstr("Shiroi Microcomputer BASIC\r\n");
	cursor();
	while(1){
		char c = getch();
		killcursor();
		if(c == '\n'){
			putstr("\r\n");
		}else{
			putchar(c);
		}
		cursor();
	}
}
