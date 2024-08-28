/* $Id$ */

#include "text.h"

#include "char.h"
#include "video.h"
#include "am9511.h"
#include "io.h"

char caps;

unsigned short posx;
unsigned short posy;
unsigned short curx;
unsigned short cury;

short text_kbd_data;

extern int scrwidth;
extern int scrheight;

extern unsigned char keylist[];
extern unsigned char keylist_caps[];

char getch(void){
	char k = 0;
rep:
	while((k = inp(text_kbd_data)) == 0);
	while(inp(text_kbd_data) != 0);
	unsigned char top = (k & 0xf0) >> 4;
	unsigned char bot = (k & 0x0f);
	top--;
	bot--;
	if(keylist[top * 13 + bot] == '!'){
		caps = caps == 0 ? 1 : 0;
		goto rep;
	}
	return caps ? keylist_caps[top * 13 + bot] : keylist[top * 13 + bot];
}

void clear(void){
	int i;
	int size = mull(scrwidth, scrheight);
	setvramaddr(0);
	for(i = 0; i < size; i++) vramchar(' ');
}

void scroll_y(void){
	int i;
	int size = mull(scrwidth, scrheight - 1);
	for(i = 0; i < size; i++){
		setreadvramaddr(i + 32);
		unsigned char ch = getvramchar();
		setvramaddr(i);
		vramchar(ch);
	}
	for(i = 0; i < scrwidth; i++){
		vramchar(' ');
	}
}

void print_ptr(void* ptr){
	unsigned short p = (unsigned short)ptr;
	int i;
	const char hex[] = "0123456789ABCDEF";
	putstr("0x");
	for(i = 0; i < 4; i++){
		putchar(hex[(p & 0xf000) >> 12]);
		p = p << 4;
	}
}

void cursor(void){
	setvramaddr(mull(posy, scrwidth) + posx);
	vramchar(248);
	curx = posx;
	cury = posy;
	if(curx == scrwidth){
		curx = 0;
		cury++;
	}
}

void killcursor(void){
	setvramaddr(mull(cury, scrwidth) + curx);
	vramchar(' ');
}

void putchar(char c){
	if(c == '\r'){
		posx = 0;
	}else if(c == '\n'){
		posy++;
	}else{
		setvramaddr(mull(posy, scrwidth) + posx);
		vramchar(c);
		posx++;
		if(posx == scrwidth){
			posx = 0;
			posy++;
		}
	}
	if(posy == scrheight){
		scroll_y();
		posy = scrheight - 1;
		posx = 0;
	}
}

void putstr(char* str){
	int i;
	for(i = 0; str[i] != 0; i++) putchar(str[i]);
}

void text_init(void){
	posx = 0;
	posy = 0;
	curx = 0;
	cury = 0;
	caps = 0;
}

void text_card(int t, int port){
	if(t == 0x22){
		text_kbd_data = port - 2;
	}
}
