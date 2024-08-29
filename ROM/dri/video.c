/* $Id$ */

#include "video.h"

#include "math.h"

#include "../char.h"
#include "../io.h"

int scrwidth;
int scrheight;

short vdp_addr;
short vdp_data;
short vdg_addr;
short vdg_data;

void write_vram(unsigned short addr){
	if(vdp_addr != -1){
		addr |= 0x4000;
		outp(vdp_addr, addr & 0x00ff);
		outp(vdp_addr, ((addr & 0xff00) >> 8));
	}
#ifndef ONLY_VDP	
	else if(vdg_addr != -1){
		outp(vdg_addr, addr & 0x00ff);
		outp(vdg_addr, ((addr & 0xff00) >> 8));
	}
#endif
}

void read_vram(unsigned short addr){
	if(vdp_addr != -1){
		outp(vdp_addr, addr & 0x00ff);
		outp(vdp_addr, ((addr & 0xff00) >> 8));
	}
#ifndef ONLY_VDP
	else if(vdg_addr != -1){
		outp(vdg_addr, addr & 0x00ff);
		outp(vdg_addr, ((addr & 0xff00) >> 8));
	}
#endif
}

void setvramaddr(unsigned short addr){
	if(vdp_addr != -1){
		write_vram(0x800 + addr);
	}
#ifndef ONLY_VDP
	else if(vdg_addr != -1){
		write_vram(addr);
	}
#endif
}

void setreadvramaddr(unsigned short addr){
	if(vdp_addr != -1){
		read_vram(0x800 + addr);
	}
#ifndef ONLY_VDP
	else if(vdg_addr != -1){
		read_vram(addr);
	}
#endif
}

void vramchar(unsigned char c){
	if(vdp_addr != -1){
		outp(vdp_data, c - 0x20);
	}
#ifndef ONLY_VDP
	else if(vdg_addr != -1){
		if(c == 248){
			outp(vdg_data, 0);
		}else{
			outp(vdg_data, toupper(c) - 'A' + 1);
		}
	}
#endif
}

void _vramchar(unsigned char c){
	if(vdp_addr != -1){
		outp(vdp_data, c);
	}
#ifndef ONLY_VDP
	else if(vdg_addr != -1){
		outp(vdg_data, c);
	}
#endif
}

unsigned char getvramchar(void){
	if(vdp_addr != -1){
		return inp(vdp_data) + 0x20;
	}
#ifndef ONLY_VDP
	else if(vdg_addr != -1){
		return inp(vdg_data) - 1 + 'A';
	}
#endif
	return 0;
}

void scroll_y(void){
	int i;
	int size = muli(scrwidth, scrheight - 1);
	for(i = 0; i < size; i++){
		setreadvramaddr(i + scrwidth);
		unsigned char ch = getvramchar();
		setvramaddr(i);
		vramchar(ch);
	}
	for(i = 0; i < scrwidth; i++){
		vramchar(' ');
	}
}

void video_init(void){
	int i;
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
#ifndef ONLY_VDP
	else if(vdg_addr != -1){
		scrwidth = 32;
		scrheight = 16;
	}
#endif
}

void video_card(int t, int port){
	if(t == 0x01){
		vdp_addr = port - 2;
		vdp_data = port - 1;
	}else if(t == 0x02){
		vdg_addr = port - 2;
		vdg_data = port - 1;
	}
}
