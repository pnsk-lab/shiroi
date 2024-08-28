/* $Id$ */
#include "shiroi_text_mk_i.h"

#include "shiroi.h"

#include <stdlib.h>

void shiroi_text_mk_i_install(shiroi_t* shiroi, int slot) {
	shiroi->cards[slot].type = SHIROI_TEXT_MARK_I;
	shiroi->cards[slot].textptr = &shiroi->cards[slot].text;

	shiroi->cards[slot].text.key = 0;
}

void shiroi_text_mk_i(shiroi_t* shiroi) {
	int i;

	uint16_t io = Z80_GET_ADDR(shiroi->z80_pins);
	uint16_t addr = io & 0xff;
	uint16_t data = (io >> 8) & 0xff;

	for(i = 0; i < 256 / SHIROI_IO_PORTS; i++) {
		if(shiroi->cards[i].type == SHIROI_TEXT_MARK_I) {
			if(shiroi->z80_pins & Z80_RD) {
				/* I/O Read */
				if(addr == 9) {
					Z80_SET_DATA(shiroi->z80_pins, shiroi->cards[i].text.key);
				} else if(addr == 11) {
					Z80_SET_DATA(shiroi->z80_pins, shiroi->cards[i].type);
				}
			} else if(shiroi->z80_pins & Z80_WR) {
				/* I/O Write */
			}
		}
	}
}

void shiroi_text_mk_i_tick(shiroi_t* shiroi) {}
