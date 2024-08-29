/* $Id$ */

#ifndef __VIDEO_H__
#define __VIDEO_H__

void video_init(void);
void video_card(int, int);

void write_vram(unsigned short);
void read_vram(unsigned short);
void setvramaddr(unsigned short);
void setreadvramaddr(unsigned short);
void vramchar(unsigned char);
void _vramchar(unsigned char);
unsigned char getvramchar(void);

#endif
