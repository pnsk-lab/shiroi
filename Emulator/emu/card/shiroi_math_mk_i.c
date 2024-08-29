/* $Id$ */
#include "shiroi_math_mk_i.h"

#include "shiroi.h"

#include "am9511.h"

void shiroi_math_mk_i_install(shiroi_t* shiroi, int slot) {
	shiroi->cards[slot].type = SHIROI_MATH_MARK_I;
	shiroi->cards[slot].mathptr = &shiroi->cards[slot].math;

	shiroi->cards[slot].math.am9511 = am_create(0, 0);
}

void shiroi_math_mk_i_reset(shiroi_t* shiroi, int slot) { am_reset(shiroi->cards[slot].math.am9511); }

void shiroi_math_mk_i(shiroi_t* shiroi) {
	int i;

	uint16_t io = Z80_GET_ADDR(shiroi->z80_pins);
	uint16_t addr = io & 0xff;
	uint16_t data = (io >> 8) & 0xff;

	for(i = 0; i < 256 / SHIROI_IO_PORTS; i++) {
		if(shiroi->cards[i].type == SHIROI_MATH_MARK_I) {
			if(shiroi->z80_pins & Z80_RD) {
				/* I/O Read */
				if(addr == 6) {
					Z80_SET_DATA(shiroi->z80_pins, am_pop(shiroi->cards[i].math.am9511));
				} else if(addr == 7) {
					Z80_SET_DATA(shiroi->z80_pins, 1);
				} else if(addr == 8) {
					Z80_SET_DATA(shiroi->z80_pins, shiroi->cards[i].type);
				}
			} else if(shiroi->z80_pins & Z80_WR) {
				/* I/O Write */
				if(addr == 6) {
					am_push(shiroi->cards[i].math.am9511, data);
				} else if(addr == 7) {
					am_command(shiroi->cards[i].math.am9511, data);
				}
			}
		}
	}
}

void shiroi_math_mk_i_tick(shiroi_t* shiroi) {}
