/* $Id$ */
#include "shiroi_sound_mk_i.h"

#include "shiroi.h"

extern float audio[];
extern int16_t incre;

void shiroi_sound_mk_i_install(shiroi_t* shiroi, int slot) {
	shiroi->cards[slot].type = SHIROI_SOUND_MARK_I;
	shiroi->cards[slot].soundptr = &shiroi->cards[slot].sound;

	ay38910_desc_t desc;
	desc.tick_hz = 3 * 1024 * 1024;
	desc.sound_hz = 48000;
	desc.magnitude = 0.5;
	desc.user_data = NULL;
	desc.in_cb = NULL;
	desc.out_cb = NULL;
	desc.type = AY38910_TYPE_8910;

	ay38910_init(&shiroi->cards[slot].sound.psg, &desc);
}

void shiroi_sound_mk_i_reset(shiroi_t* shiroi, int slot) { ay38910_reset(&shiroi->cards[slot].sound.psg); }

void shiroi_sound_mk_i(shiroi_t* shiroi) {
	int i;

	uint16_t io = Z80_GET_ADDR(shiroi->z80_pins);
	uint16_t addr = io & 0xff;
	uint16_t data = (io >> 8) & 0xff;

	for(i = 0; i < 256 / SHIROI_IO_PORTS; i++) {
		if(shiroi->cards[i].type == SHIROI_SOUND_MARK_I) {
			if(shiroi->z80_pins & Z80_RD) {
				/* I/O Read */
				if(addr == 5) {
					Z80_SET_DATA(shiroi->z80_pins, shiroi->cards[i].type);
				}
			} else if(shiroi->z80_pins & Z80_WR) {
				/* I/O Write */
				if(addr == 3) {
					uint64_t pins;
					pins = AY38910_BDIR | AY38910_BC1;
					AY38910_SET_DATA(pins, data);
					ay38910_iorq(&shiroi->cards[i].sound.psg, pins);
				} else if(addr == 4) {
					uint64_t pins;
					pins = AY38910_BDIR;
					AY38910_SET_DATA(pins, data);
					ay38910_iorq(&shiroi->cards[i].sound.psg, pins);
				}
			}
		}
	}
}

void shiroi_sound_mk_i_tick(shiroi_t* shiroi) {
	int i;
	for(i = 0; i < 256 / SHIROI_IO_PORTS; i++) {
		if(shiroi->cards[i].type == SHIROI_SOUND_MARK_I) {
			if(ay38910_tick(&shiroi->cards[i].sound.psg)) {
				audio[incre++] = shiroi->cards[i].sound.psg.sample;
				if(incre == 480) incre = 0;
			}
		}
	}
}
