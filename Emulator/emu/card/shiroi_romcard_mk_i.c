/* $Id$ */
#include "shiroi_romcard_mk_i.h"

#include "shiroi.h"

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

void shiroi_romcard_mk_i_install(shiroi_t* shiroi, int slot) {
	shiroi->cards[slot].type = SHIROI_ROMCARD_MARK_I;
	shiroi->cards[slot].romcard.data = malloc(1 << 24);
	if(shiroi->cards[slot].romcard.data == NULL) {
		fprintf(stderr, "Could not allocate!\n");
#ifndef __MINGW32__
		raise(SIGABRT);
#endif
	}
	shiroi->cards[slot].romcardptr = &shiroi->cards[slot].romcard;
}

void shiroi_romcard_mk_i_reset(shiroi_t* shiroi, int slot) {}

void shiroi_romcard_mk_i(shiroi_t* shiroi) {
	int i;

	uint16_t io = Z80_GET_ADDR(shiroi->z80_pins);
	uint16_t addr = io & 0xff;
	uint16_t data = (io >> 8) & 0xff;

	for(i = 0; i < 256 / SHIROI_IO_PORTS; i++) {
		if(shiroi->cards[i].type == SHIROI_ROMCARD_MARK_I) {
			if(shiroi->z80_pins & Z80_RD) {
				/* I/O Read */
				if(addr == 16) {
					Z80_SET_DATA(shiroi->z80_pins, shiroi->cards[i].romcard.data[shiroi->cards[i].romcard.latch_addr]);
				} else if(addr == 17) {
					Z80_SET_DATA(shiroi->z80_pins, shiroi->cards[i].type);
				}
			} else if(shiroi->z80_pins & Z80_WR) {
				/* I/O Write */
				if(addr == 15) {
					shiroi->cards[i].romcard.latch_addr <<= 8;
					shiroi->cards[i].romcard.latch_addr |= data;
				}
			}
		}
	}
}

void shiroi_romcard_mk_i_tick(shiroi_t* shiroi) {}
