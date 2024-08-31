/* $Id$ */
#include "shiroi_debug.h"

#include "shiroi.h"

#include <stdlib.h>

void shiroi_debug_install(shiroi_t* shiroi, int slot) {
	shiroi->cards[slot].type = SHIROI_DEBUG;
	shiroi->cards[slot].debugptr = &shiroi->cards[slot].debug;

	int i;
	for(i = 0; i < 4; i++) shiroi->cards[slot].debug.latch[i] = 0xff;
}

void shiroi_debug_reset(shiroi_t* shiroi, int slot) {
	shiroi->cards[slot].text.key = 0;
	shiroi->cards[slot].text.caps = false;
}

void shiroi_debug(shiroi_t* shiroi) {
	int i;

	uint16_t io = Z80_GET_ADDR(shiroi->z80_pins);
	uint16_t addr = io & 0xff;
	uint16_t data = (io >> 8) & 0xff;

	for(i = 0; i < 256 / SHIROI_IO_PORTS; i++) {
		if(shiroi->cards[i].type == SHIROI_DEBUG) {
			if(shiroi->z80_pins & Z80_RD) {
				/* I/O Read */
				if(addr == 14) {
					Z80_SET_DATA(shiroi->z80_pins, shiroi->cards[i].type);
				}
			} else if(shiroi->z80_pins & Z80_WR) {
				/* I/O Write */
				if(addr == 12) {
					printf("!\n");
					shiroi->cards[i].debug.latch_addr = data;
				} else if(addr == 13) {
					shiroi->cards[i].debug.latch[shiroi->cards[i].debug.latch_addr] = data;
				}
			}
		}
	}
}

void shiroi_debug_tick(shiroi_t* shiroi) {}
