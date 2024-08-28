/* $Id$ */

unsigned char inp(unsigned char);
void outp(unsigned char, unsigned char);
void write_vram(unsigned short);
void putchar(char);
void putstr(char*);
void scroll_y(void);
void beep(void);
void _beep(unsigned long howlong);
void init_cards(void);
void print_ptr(void*);
char getch(void);
void clear(void);
void basic(void);
int strlen(const char*);
void cursor(void);
void killcursor(void);
char toupper(char c);
long modl(long a, long b);
long divl(long a, long b);
long mull(long a, long b);

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

unsigned short posx;
unsigned short posy;
unsigned short curx;
unsigned short cury;

short vdp_addr;
short vdp_data;
short vdg_addr;
short vdg_data;
short psg_addr;
short psg_data;
short fpu_stack;
short fpu_command;
short text_kbd_data;

int scrwidth;
int scrheight;

char caps;

unsigned char keylist[13 * 4];
unsigned char keylist_caps[13 * 4];

unsigned char basicbuffer[1024 * 30];

void main(void){
	int i;
	caps = 0;

/*
 * / 1 2 3 4 5 6 7 8 9 10 11 12 13
 * 1 1 2 3 4 5 6 7 8 9 0  -  =  bs
 * 2 q w e r t y u i o p  [  ]  rt
 * 3 a s d f g h j k l ;  '  \  cl
 * 4 z x c v b n m , . /  sp
 *
 * When Caps Lock
 *
 * / 1 2 3 4 5 6 7 8 9 10 11 12 13
 * 1 ! @ # $ % ^ & * ( )  _  +  bs
 * 2 Q W E R T Y U I O P  {  }  rt
 * 3 A S D F G H J K L :  "  |  cl
 * 4 Z X C V B N M < > ?  sp
 */
	const char* keys;

	keys = "1234567890-=\x08";
	for(i = 0; i < 13; i++){
		keylist[i] = keys[i];
	}
	keys = "qwertyuiop[]\n";
	for(i = 0; i < 13; i++){
		keylist[13 + i] = keys[i];
	}
	keys = "asdfghjkl;'\\!";
	for(i = 0; i < 13; i++){
		keylist[26 + i] = keys[i];
	}
	keys = "zxcvbnm,./    ";
	for(i = 0; i < 13; i++){
		keylist[39 + i] = keys[i];
	}

	keys = "!@#$%^&*()_+\x08";
	for(i = 0; i < 13; i++){
		keylist_caps[i] = keys[i];
	}
	keys = "QWERTYUIOP{}\n";
	for(i = 0; i < 13; i++){
		keylist_caps[13 + i] = keys[i];
	}
	keys = "ASDFGHJKL:\"|!";
	for(i = 0; i < 13; i++){
		keylist_caps[26 + i] = keys[i];
	}
	keys = "ZXCVBNM<>?    ";
	for(i = 0; i < 13; i++){
		keylist_caps[39 + i] = keys[i];
	}

	posx = 0;
	posy = 0;
	curx = 0;
	cury = 0;

	vdp_addr = -1;
	vdg_addr = -1;
	psg_addr = -1;
	fpu_stack = -1;
	text_kbd_data = -1;

	init_cards();

	if(vdp_addr == -1 && vdg_addr == -1){
		int i;
		for(i = 0; i < 3; i++){
			_beep(3L * 1024);
			unsigned long j;
			for(j = 0; j < 3L * 1024; j++);
		}
		while(1);
	}

	if(vdp_addr != -1){	
		scrwidth = 32;
		scrheight = 24;

		outp(vdp_addr, 0x00);
		outp(vdp_addr, 0x80);
	
		outp(vdp_addr, 0xd8);
		outp(vdp_addr, 0x81);
	
		outp(vdp_addr, 0x02);
		outp(vdp_addr, 0x82);
	
		outp(vdp_addr, 0x50);
		outp(vdp_addr, 0x83);
	
		outp(vdp_addr, 0x00);
		outp(vdp_addr, 0x84);
	
		outp(vdp_addr, 0x29);
		outp(vdp_addr, 0x85);
	
		outp(vdp_addr, 0x03);
		outp(vdp_addr, 0x86);
	
		outp(vdp_addr, 0xe4);
		outp(vdp_addr, 0x87);
	}else if(vdg_addr != -1){
		scrwidth = 32;
		scrheight = 16;
	}

	if(vdp_addr != -1){
		/*
		 * VDP:
		 * 0x0000-0x0800: font
		 * 0x0800-0x1400: pattern
		 * 0x1400-0x1480: color
		 * 0x1480-0x1500: sprite attr
		 * 0x1800-0x2000: sprite pattern
		 */
	
		write_vram(0);
		for(i = 0; i < 0x800; i++) outp(vdp_data, *((unsigned char*)(0x6000 - 2048 + i)));
	
		write_vram(0x1400);
		for(i = 0; i < 0x20; i++) outp(vdp_data, 0xf0);
		write_vram(0x1400 + 0x10);
		for(i = 0; i < 0x10; i++) outp(vdp_data, ((i & 0xf) << 4) | 0);
	}

	clear();

//	beep();

	if(text_kbd_data == -1){
		putstr("Text  Card Mark I not present\r\n");
		putstr("Text  Card Mark I is required to use the BASIC\r\n");
		putstr("Halted. Get one.\r\n");
		while(1);
	}
	if(fpu_stack == -1){
		putstr("Math  Card Mark I not present\r\n");
		putstr("Math  Card Mark I is required to use the BASIC\r\n");
		putstr("Halted. Get one.\r\n");
		while(1);
	}

	int incr;
	int move = 0;
	char k;
	int wait = 0;

	const char* title;

	if(vdp_addr != -1){
		title = "Shiroi Microcomputer";
		write_vram(0x800 + scrwidth / 2 - strlen(title) / 2 + mull(scrheight / 2 - 1, scrwidth));
		for(i = 0; title[i] != 0; i++) outp(vdp_data, title[i] - 0x20);

		title = "\x82 2024 Nishi";
		write_vram(0x800 + scrwidth / 2 - strlen(title) / 2 + mull(scrheight - 2, scrwidth));
		for(i = 0; title[i] != 0; i++) outp(vdp_data, title[i] - 0x20);

		title = "Press any key to begin";
		write_vram(0x800 + scrwidth / 2 - strlen(title) / 2 + mull(scrheight / 2 + 1, scrwidth));
		for(i = 0; title[i] != 0; i++) outp(vdp_data, title[i] - 0x20);
	}else if(vdg_addr != -1){
		title = "Shiroi Microcomputer";
		write_vram(scrwidth / 2 - strlen(title) / 2 + mull(scrheight / 2 - 1, scrwidth));
		for(i = 0; title[i] != 0; i++) outp(vdg_data, toupper(title[i]) - 'A' + 1);

		title = "(C) 2024 Nishi";
		write_vram(scrwidth / 2 - strlen(title) / 2 + mull(scrheight - 2, scrwidth));
		for(i = 0; title[i] != 0; i++) outp(vdg_data, toupper(title[i]) - 'A' + 1);

		title = "Press any key to begin";
		write_vram(scrwidth / 2 - strlen(title) / 2 + mull(scrheight / 2 + 1, scrwidth));
		for(i = 0; title[i] != 0; i++) outp(vdg_data, toupper(title[i]) - 'A' + 1);
	}

move_bar:
	if(vdp_addr != -1){
	}else if(vdg_addr != -1){
		goto skip;
	}
	incr = move;
	for(i = 0; i < 16; i++){
		int j;
		for(j = 0; j < 3; j++){
			int p = incr;
			if(p >= 15) p = p - 15;
			if(wait == 0){
				write_vram(0x800 + i * 2 + j * 32);
				outp(vdp_data, (p + 1) * 8 + 0x80);
				outp(vdp_data, (p + 1) * 8 + 0x80);
				write_vram(0x800 + i * 2 + 32 * 20 - j * 32);
				outp(vdp_data, (p + 1) * 8 + 0x80);
				outp(vdp_data, (p + 1) * 8 + 0x80);
			}else{
				if(i * 2 - 1 < 0){
					write_vram(0x800 + i * 2 + j * 32);
					outp(vdp_data, (p + 1) * 8 + 0x80);
				}else{
					write_vram(0x800 + i * 2 + j * 32 - 1);
					outp(vdp_data, (p + 1) * 8 + 0x80);
					outp(vdp_data, (p + 1) * 8 + 0x80);
				}
				if(i * 2 - 1 < 0){
					write_vram(0x800 + i * 2 + 32 * 20 - j * 32);
					outp(vdp_data, (p + 1) * 8 + 0x80);
				}else{
					write_vram(0x800 + i * 2 + 32 * 20 - j * 32 - 1);
					outp(vdp_data, (p + 1) * 8 + 0x80);
					outp(vdp_data, (p + 1) * 8 + 0x80);
				}
			}
		}
		incr++;
		if(incr == 15) incr = 0;
	}
	wait++;
	if(wait == 2){
		wait = 0;
		move++;
		if(move == 15) move = 0;
	}
	for(i = 0; i < 3 * 1024; i++);

skip:
	if((k = inp(text_kbd_data)) == 0) goto move_bar;
	while(inp(text_kbd_data) != 0);

	basic();
}

void basic(void){
	clear();

	if(vdp_addr == -1){
		outp(vdp_addr, 0xf0);
		outp(vdp_addr, 0x87);
	}

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

int strlen(const char* str){
	int i;
	for(i = 0; str[i] != 0; i++);
	return i;
}

void clear(void){
	int i;
	int size = mull(scrwidth, scrheight);
	if(vdp_addr != -1){
		write_vram(0x800);
		for(i = 0; i < size; i++) outp(vdp_data, 0);
	}else if(vdg_addr != -1){
		write_vram(0);
		for(i = 0; i < size; i++){
			outp(vdg_data, 0x20);
		}
	}
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
	}
	return caps ? keylist_caps[top * 13 + bot] : keylist[top * 13 + bot];
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

void init_cards(void){
	int i;
	int port = 2;
	for(i = 0; i < 256 / 3; i++){
		int t = inp(port);
		if(t != 0){
			if(t == 0x01){
				vdp_addr = port - 2;
				vdp_data = port - 1;
			}else if(t == 0x02){
				vdg_addr = port - 2;
				vdg_data = port - 1;
			}else if(t == 0x11){
				psg_addr = port - 2;
				psg_data = port - 1;
			}else if(t == 0x21){
				fpu_stack = port - 2;
				fpu_command = port - 1;
			}else if(t == 0x22){
				text_kbd_data = port - 2;
			}

		}
		port += 3;
	}
}

void beep(void){
	_beep(3L * 1024);
}

void _beep(unsigned long howlong){
	if(psg_addr == -1) return;
	unsigned long i;

	outp(psg_addr, 8);
	outp(psg_data, 0x0f);

	outp(psg_addr, 0);
	outp(psg_data, 0xd6);

	outp(psg_addr, 1);
	outp(psg_data, 0x0);

	outp(psg_addr, 7);
	outp(psg_data, 0x3e);

	for(i = 0; i < howlong; i++);

	outp(psg_addr, 7);
	outp(psg_data, 0x3f);
}

void write_vram(unsigned short addr){
	if(vdp_addr != -1){
		addr |= 0x4000;
		outp(vdp_addr, addr & 0x00ff);
		outp(vdp_addr, ((addr & 0xff00) >> 8));
	}else if(vdg_addr != -1){
		outp(vdg_addr, addr & 0x00ff);
		outp(vdg_addr, ((addr & 0xff00) >> 8));
	}
}

void read_vram(unsigned short addr){
	if(vdp_addr != -1){
		outp(vdp_addr, addr & 0x00ff);
		outp(vdp_addr, ((addr & 0xff00) >> 8));
	}else{
		outp(vdg_addr, addr & 0x00ff);
		outp(vdg_addr, ((addr & 0xff00) >> 8));
	}
}

void scroll_y(void){
	int i;
	int size = mull(scrwidth, scrheight - 1);
	for(i = 0; i < size; i++){
		if(vdp_addr != -1){
			read_vram(0x800 + i + 32);
			unsigned char ch = inp(vdp_data);
			write_vram(0x800 + i);
			outp(vdp_data, ch);
		}else if(vdg_addr != -1){
			read_vram(i + 32);
			unsigned char ch = inp(vdg_data);
			write_vram(i);
			outp(vdg_data, ch);
		}
	}
	for(i = 0; i < scrwidth; i++){
		if(vdp_addr != -1){
			outp(vdp_data, 0);
		}else if(vdg_addr != -1){
			outp(vdg_data, 0x20);
		}
	}
}

void cursor(void){
	if(vdp_addr != -1){
		write_vram(0x800 + mull(posy, scrwidth) + posx);
		outp(vdp_data, 248);
	}else if(vdg_addr != -1){
		write_vram(mull(posy, scrwidth) + posx);
		outp(vdg_data, 0);
	}
	curx = posx;
	cury = posy;
	if(curx == scrwidth){
		curx = 0;
		cury++;
	}
}

void killcursor(void){
	if(vdp_addr != -1){
		write_vram(0x800 + mull(cury, scrwidth) + curx);
		outp(vdp_data, 0);
	}else if(vdg_addr != -1){
		write_vram(mull(cury, scrwidth) + curx);
		outp(vdg_data, 0x20);
	}
}

char toupper(char c){
	if('a' <= c && c <= 'z'){
		return c - 'a' + 'A';
	}
	return c;
}

void putchar(char c){
	if(c == '\r'){
		posx = 0;
	}else if(c == '\n'){
		posy++;
	}else{
		if(vdp_addr != -1){
			write_vram(0x800 + mull(posy, scrwidth) + posx);
		}else if(vdg_addr != -1){
			write_vram(mull(posy, scrwidth) + posx);
		}
		if(vdp_addr != -1){
			outp(vdp_data, c - 0x20);
		}else if(vdg_addr != -1){
			outp(vdg_data, toupper(c) - 'A' + 1);
		}
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
	long r = 0;
	for(i = 0; i < 4; i++){
		r = r << 8;
		r |= inp(fpu_stack);
	}
	return r;
}


unsigned char inp(unsigned char port) __naked {
__asm
	ld      hl,#2
	add     hl,sp
	ld      c,(hl)
	in      a,(c)
	ld      l,a
	ld      h,#0
	ret
__endasm;
}

void outp(unsigned char port, unsigned char data) __naked {
__asm
	push bc
	ld      hl,#4
	add     hl,sp
	ld      c,(hl)
	inc     hl
	ld      b,(hl)
	out     (c),b
	pop     bc
	ret
__endasm;
}
