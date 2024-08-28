/* $Id$ */

#include "io.h"
#include "math.h"
#include "sound.h"
#include "video.h"
#include "text.h"
#include "char.h"

void init_cards(void);
void basic(void);

extern short vdp_addr;
extern short vdp_data;
extern short vdg_addr;
extern short vdg_data;

extern short psg_addr;
extern short psg_data;

extern short fpu_stack;
extern short fpu_command;

extern short text_kbd_data;

extern int scrwidth;
extern int scrheight;

extern char caps;

unsigned char keylist[13 * 4];
unsigned char keylist_caps[13 * 4];

void main(void){
	int i;

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

	vdp_addr = -1;
	vdg_addr = -1;
	psg_addr = -1;
	fpu_stack = -1;
	text_kbd_data = -1;

	init_cards();

#ifdef ONLY_VDP
	if(vdp_addr == -1){
#else
	if(vdp_addr == -1 && vdg_addr == -1){
#endif
		int i;
		for(i = 0; i < 3; i++){
			_beep(3L * 1024);
			unsigned long j;
			for(j = 0; j < 3L * 1024; j++);
		}
		while(1);
	}

	video_init();
	text_init();

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

#ifdef ONLY_VDP
	if(vdp_addr != -1){
#else
	if(vdp_addr != -1 || vdg_addr != -1){
#endif
		title = "Shiroi Microcomputer";
		setvramaddr(scrwidth / 2 - strlen(title) / 2 + mull((scrheight - 9) / 2 - 1 + 3, scrwidth));
		for(i = 0; title[i] != 0; i++) vramchar(title[i]);

		if(vdp_addr != -1){
			title = "\x82 2024 Nishi";
		}else if(vdg_addr != -1){
			title = "(C) 2024 Nishi";
		}
		setvramaddr(scrwidth / 2 - strlen(title) / 2 + mull(scrheight - 2, scrwidth));
		for(i = 0; title[i] != 0; i++) vramchar(title[i]);

		title = "Press any key to begin";
		setvramaddr(scrwidth / 2 - strlen(title) / 2 + mull((scrheight - 9) / 2 + 1 + 3, scrwidth));
		for(i = 0; title[i] != 0; i++) vramchar(title[i]);
	}

move_bar:
	if(vdp_addr != -1){
	}
#ifndef ONLY_VDP
	else if(vdg_addr != -1){
		goto skip;
	}
#endif
	incr = move;
	for(i = 0; i < 16; i++){
		int j;
		for(j = 0; j < 3; j++){
			int p = incr;
			if(p >= 15) p = p - 15;
			if(wait == 0){
				setvramaddr(i * 2 + j * 32);
				_vramchar((p + 1) * 8 + 0x80);
				_vramchar((p + 1) * 8 + 0x80);
				setvramaddr(i * 2 + 32 * 20 - j * 32);
				_vramchar((p + 1) * 8 + 0x80);
				_vramchar((p + 1) * 8 + 0x80);
			}else{
				if(i * 2 - 1 < 0){
					setvramaddr(i * 2 + j * 32);
					_vramchar((p + 1) * 8 + 0x80);
				}else{
					setvramaddr(i * 2 + j * 32 - 1);
					_vramchar((p + 1) * 8 + 0x80);
					_vramchar((p + 1) * 8 + 0x80);
				}
				if(i * 2 - 1 < 0){
					setvramaddr(i * 2 + 32 * 20 - j * 32);
					_vramchar((p + 1) * 8 + 0x80);
				}else{
					setvramaddr(i * 2 + 32 * 20 - j * 32 - 1);
					_vramchar((p + 1) * 8 + 0x80);
					_vramchar((p + 1) * 8 + 0x80);
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

	if(vdp_addr == -1){
		outp(vdp_addr, 0xf0);
		outp(vdp_addr, 0x87);
	}

	basic();
}

void init_cards(void){
	int i;
	int port = 2;
	for(i = 0; i < 256 / 3; i++){
		int t = inp(port);
		if(t != 0){
			video_card(t, port);
			sound_card(t, port);
			math_card(t, port);
			text_card(t, port);
		}
		port += 3;
	}
}
