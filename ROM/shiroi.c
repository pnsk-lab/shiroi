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

unsigned short posx;
unsigned short posy;

short vdp_addr;
short vdp_data;
short psg_addr;
short psg_data;
short fpu_addr;
short fpu_data;

void main(void){
	int i;

	posx = 0;
	posy = 0;

	vdp_addr = -1;
	psg_addr = -1;
	fpu_addr = -1;

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
	for(i = 0; i < 0x800; i++) outp(vdp_data, *((unsigned char*)(0x2000 + i)));

	beep();

	putstr("Shiroi Microcomputer BASIC\r\n");
	if(fpu_addr == -1){
		putstr("Math Card Mark I not present\r\n");
	}else{
		putstr("Math Card Mark I present\r\n");
	}
	if(psg_addr == -1){
		putstr("Sound Card Mark I not present\r\n");
	}else{
		putstr("Sound Card Mark I present\r\n");
	}

	write_vram(0x800 + 40 * 12);
	for(i = 0; i < 0x100; i++) outp(vdp_data, i);

	while(1);
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
