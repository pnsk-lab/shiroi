/* $Id$ */

#include "text.h"

#include "video.h"
#include "math.h"

#include "../char.h"
#include "../io.h"

char caps;

short posx;
short posy;
short curx;
short cury;
unsigned char cursorcolor;
unsigned char cursorindex;

short text_kbd_data;

extern int scrwidth;
extern int scrheight;

extern unsigned char keylist[];
extern unsigned char keylist_caps[];

char agetch(void){
	char k;
rep:
	if((k = inp(text_kbd_data)) == 0) return 0;
	while(inp(text_kbd_data) != 0);
	unsigned char top = (k & 0xf0) >> 4;
	unsigned char bot = (k & 0x0f);
	top--;
	bot--;
	if(keylist[top * 13 + bot] == '!'){
		caps = caps == 0 ? 1 : 0;
		goto rep;
	}else if(keylist[top * 13 + bot] == '?'){
		return 1;
	}
	return caps ? keylist_caps[top * 13 + bot] : keylist[top * 13 + bot];
}

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
	}else if(keylist[top * 13 + bot] == '?'){
		return 1;
	}
	return caps ? keylist_caps[top * 13 + bot] : keylist[top * 13 + bot];
}

void clear(void){
	int i;
	int size = muli(scrwidth, scrheight);
	setvramaddr(0);
	for(i = 0; i < size; i++) vramchar(' ');
	posx = 0;
	posy = 0;
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

unsigned short charbuf;

void cursor(void){
	setvramaddr(muli(posy, scrwidth) + posx);
	_vramchar(136 + 8 * cursorcolor);
	cursorindex++;
	if(cursorindex == 2){
		cursorcolor++;
		cursorindex = 0;
		if(cursorcolor == 15) cursorcolor = 0;
	}
	curx = posx;
	cury = posy;
	if(curx == scrwidth){
		curx = 0;
		cury++;
	}
}

void killcursor(void){
	setvramaddr(muli(cury, scrwidth) + curx);
	if(charbuf == 0xffff){
		vramchar(' ');
	}else{
		vramchar(charbuf & 0xff);
	}
}

void putchar(char c){
	if(c == '\r'){
		posx = 0;
	}else if(c == '\n'){
		posy++;
	}else if(c == 0x08){
		if(posx > 0){
			posx--;
		}else{
			posy--;
			posx = scrwidth - 1;
		}
		setreadvramaddr(muli(posy, scrwidth) + posx);
		charbuf = getvramchar();
	}else{
		setvramaddr(muli(posy, scrwidth) + posx);
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

void putnum(int num){
	int i;
	char buffer[513];
	buffer[512] = 0;
	int incr = 511;
	char m = num < 0 ? 1 : 0;
	while(1){
		int l = modl(num, 10);
		if(l < 0) l = -l;
		buffer[incr--] = '0' + l;
		num = divl(num, 10);
		if(num == 0) break;
	}
	if(m == 1){
		buffer[incr--] = '-';
	}
	putstr(buffer + incr + 1);
}

void text_init(void){
	posx = 0;
	posy = 0;
	curx = 0;
	cury = 0;
	caps = 0;
	cursorcolor = 0;
	cursorindex = 0;
	charbuf = 0xffff;
}

void text_card(int t, int port){
	if(t == 0x22){
		text_kbd_data = port - 2;
	}
}
