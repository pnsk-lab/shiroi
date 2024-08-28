/* $Id$ */
#include "shiroi_video_mk_i.h"

#include "shiroi.h"

#include "tms9918util.h"

#include <stdlib.h>

void shiroi_video_mk_i_install(shiroi_t* shiroi, int slot) {
	shiroi->cards[slot].type = SHIROI_VIDEO_MARK_I;
	shiroi->cards[slot].videoptr = &shiroi->cards[slot].video;

	shiroi->cards[slot].video.width = TMS9918_PIXELS_X;
	shiroi->cards[slot].video.height = TMS9918_PIXELS_Y;
	shiroi->cards[slot].video.tick = 0;

	shiroi->cards[slot].video.vdp = vrEmuTms9918New();
	int x, y;
	for(y = 0; y < TMS9918_PIXELS_Y; y++) {
		for(x = 0; x < TMS9918_PIXELS_X; x++) {
			shiroi->cards[slot].video.fb[y * TMS9918_PIXELS_X + x] = 0;
		}
	}
}

void shiroi_video_mk_i(shiroi_t* shiroi) {
	int i;

	uint16_t io = Z80_GET_ADDR(shiroi->z80_pins);
	uint16_t addr = io & 0xff;
	uint16_t data = (io >> 8) & 0xff;

	for(i = 0; i < 256 / SHIROI_IO_PORTS; i++) {
		if(shiroi->cards[i].type == SHIROI_VIDEO_MARK_I) {
			if(shiroi->z80_pins & Z80_RD) {
				/* I/O Read */
				if(addr == 1) {
					Z80_SET_DATA(shiroi->z80_pins, vrEmuTms9918ReadData(shiroi->cards[i].video.vdp));
				} else if(addr == 2) {
					Z80_SET_DATA(shiroi->z80_pins, shiroi->cards[i].type);
				}
			} else if(shiroi->z80_pins & Z80_WR) {
				/* I/O Write */
				if(addr == 0) {
					vrEmuTms9918WriteAddr(shiroi->cards[i].video.vdp, data);
				} else if(addr == 1) {
					vrEmuTms9918WriteData(shiroi->cards[i].video.vdp, data);
				}
			}
		}
	}
}

void shiroi_video_mk_i_tick(shiroi_t* shiroi) {
	uint8_t scanline[TMS9918_PIXELS_X];
	int i, y, x;
	for(i = 0; i < 256 / SHIROI_IO_PORTS; i++) {
		if(shiroi->cards[i].type == SHIROI_VIDEO_MARK_I) {
			shiroi->cards[i].video.tick++;
			if(shiroi->cards[i].video.tick == 256) {
				shiroi->cards[i].video.tick = 0;
				for(y = 0; y < TMS9918_PIXELS_Y; y++) {
					vrEmuTms9918ScanLine(shiroi->cards[i].video.vdp, y, scanline);
					for(x = 0; x < TMS9918_PIXELS_X; x++) {
						shiroi->cards[i].video.fb[y * TMS9918_PIXELS_X + x] = vrEmuTms9918Palette[scanline[x]];
					}
				}
			}
		}
	}
}
