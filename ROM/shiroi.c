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

unsigned short posx;
unsigned short posy;

short vdp_addr;
short vdp_data;
short psg_addr;
short psg_data;
short fpu_addr;
short fpu_data;
short text_kbd_data;

char caps;

unsigned char keylist[13 * 4];
unsigned char keylist_caps[13 * 4];

unsigned char basicbuffer[1024 * 30];

void main(void){
	int i;
	caps = 0;

/*
 * / 1 2 3 4 5 6 7 8 9 10 11 12 13
 * 1 1 2 3 4 5 6 7 8 9 0  -  =  
 * 2 q w e r t y u i o p  [  ]  rt
 * 3 a s d f g h j k l ;  '  \  cl
 * 4 z x c v b n m , . /  sp
 */
	const char* keys;

	keys = "1234567890-= ";
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

	keys = "!@#$%^&*()_+ ";
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

	vdp_addr = -1;
	psg_addr = -1;
	fpu_addr = -1;
	text_kbd_data = -1;

	init_cards();

	if(vdp_addr == -1){
		int i;
		for(i = 0; i < 3; i++){
			_beep(3L * 1024);
			unsigned long j;
			for(j = 0; j < 3L * 1024; j++);
		}
		while(1);
	}
	
	outp(vdp_addr, 0x00);
	outp(vdp_addr, 0x80);

	outp(vdp_addr, 0xd0);
	outp(vdp_addr, 0x81);

	outp(vdp_addr, 0x02);
	outp(vdp_addr, 0x82);

	outp(vdp_addr, 0x00);
	outp(vdp_addr, 0x84);

	outp(vdp_addr, 0xf4);
	outp(vdp_addr, 0x87);

	write_vram(0);
	for(i = 0; i < 0x800; i++) outp(vdp_data, *((unsigned char*)(0x6000 - 2048 + i)));

//	beep();

	putstr("Shiroi Microcomputer BASIC\r\n");
	if(psg_addr == -1){
		putstr("Sound Card Mark I not present\r\n");
	}else{
		putstr("Sound Card Mark I     present\r\n");
	}
	if(fpu_addr == -1){
		putstr("Math  Card Mark I not present\r\n");
	}else{
		putstr("Math  Card Mark I     present\r\n");
	}
	if(text_kbd_data == -1){
		putstr("Text  Card Mark I not present\r\n");
		putstr("Text  Card Mark I is required to use the BASIC\r\n");
		putstr("Halted. Get one.\r\n");
		while(1);
	}else{
		putstr("Text  Card Mark I     present\r\n");
	}

	while(1){
		putchar(getch());
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
			}else if(t == 0x11){
				psg_addr = port - 2;
				psg_data = port - 1;
			}else if(t == 0x21){
				fpu_addr = port - 2;
				fpu_data = port - 1;
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
	addr |= 0x4000;
	outp(vdp_addr, addr & 0x00ff);
	outp(vdp_addr, ((addr & 0xff00) >> 8));
}

void read_vram(unsigned short addr){
	outp(vdp_addr, addr & 0x00ff);
	outp(vdp_addr, ((addr & 0xff00) >> 8));
}

void scroll_y(void){
	int i;
	for(i = 0; i < 40 * 23; i++){
		read_vram(0x800 + i + 40);
		unsigned char ch = inp(vdp_data);
		write_vram(0x800 + i);
		outp(vdp_data, ch);
	}
	for(i = 0; i < 40; i++){
		outp(vdp_data, 0);
	}
}

void putchar(char c){
	if(c == '\r'){
		posx = 0;
	}else if(c == '\n'){
		posy++;
	}else{
		write_vram(0x800 + posy * 40 + posx);
		outp(vdp_data, c);
		posx++;
		if(posx == 40){
			posx = 0;
			posy++;
		}
	}
	if(posy == 24){
		scroll_y();
		posy = 23;
		posx = 0;
	}
}

void putstr(char* str){
	int i;
	for(i = 0; str[i] != 0; i++) putchar(str[i]);
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
