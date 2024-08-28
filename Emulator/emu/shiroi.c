/* $Id$ */

#include "shiroi.h"

#include "card/shiroi_video_mk_i.h"
#include "card/shiroi_video_mk_ii.h"
#include "card/shiroi_sound_mk_i.h"
#include "card/shiroi_math_mk_i.h"
#include "card/shiroi_text_mk_i.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

float audio[480];
int16_t incre = 0;

void shiroi_play_audio(void* buffer, unsigned int frames) {
	int16_t* d = (int16_t*)buffer;
	int i;
	for(i = 0; i < frames; i++) {
		d[i] = audio[(i + incre >= 480) ? (i + incre - 480) : (i + incre)] * 32767;
	}
}

shiroi_card_t* shiroi_get_video_card(shiroi_t* shiroi) {
	int i;
	for(i = 0; i < 256 / SHIROI_IO_PORTS; i++) {
		if(shiroi->cards[i].type == 0) continue;
		if((shiroi->cards[i].type & 0xf0) == SHIROI_VIDEO) return &shiroi->cards[i];
	}
	return NULL;
}

shiroi_card_t* shiroi_get_sound_card(shiroi_t* shiroi) {
	int i;
	for(i = 0; i < 256 / SHIROI_IO_PORTS; i++) {
		if(shiroi->cards[i].type == 0) continue;
		if((shiroi->cards[i].type & 0xf0) == SHIROI_SOUND) return &shiroi->cards[i];
	}
	return NULL;
}

shiroi_card_t* shiroi_get_math_card(shiroi_t* shiroi) {
	int i;
	for(i = 0; i < 256 / SHIROI_IO_PORTS; i++) {
		if(shiroi->cards[i].type == 0) continue;
		if(shiroi->cards[i].type == SHIROI_MATH_MARK_I) return &shiroi->cards[i];
	}
	return NULL;
}

shiroi_card_t* shiroi_get_text_card(shiroi_t* shiroi) {
	int i;
	for(i = 0; i < 256 / SHIROI_IO_PORTS; i++) {
		if(shiroi->cards[i].type == 0) continue;
		if(shiroi->cards[i].type == SHIROI_TEXT_MARK_I) return &shiroi->cards[i];
	}
	return NULL;
}

void shiroi_init_cards(shiroi_t* shiroi) {
	int i;
	for(i = 0; i < 256 / SHIROI_IO_PORTS; i++) {
		shiroi->cards[i].type = 0;
	}
}

void shiroi_install(shiroi_t* shiroi, int slot, int card) {
	if(card == SHIROI_VIDEO_MARK_I) {
		shiroi_video_mk_i_install(shiroi, slot);
	} else if(card == SHIROI_VIDEO_MARK_II) {
		shiroi_video_mk_ii_install(shiroi, slot);
	} else if(card == SHIROI_SOUND_MARK_I) {
		shiroi_sound_mk_i_install(shiroi, slot);
	} else if(card == SHIROI_MATH_MARK_I) {
		shiroi_math_mk_i_install(shiroi, slot);
	} else if(card == SHIROI_TEXT_MARK_I) {
		shiroi_text_mk_i_install(shiroi, slot);
	}
}

void shiroi_init(shiroi_t* shiroi) {
	shiroi->z80_pins = z80_init(&shiroi->z80);
	shiroi->stop = false;
	shiroi->play_audio = shiroi_play_audio;

	int i;

	for(i = 0; i < 480; i++) {
		audio[i] = 0;
	}
}

void shiroi_loop(shiroi_t* shiroi) {
	int x = 0;
	int y = 0;
	while(!shiroi->stop) {
		shiroi->z80_pins = z80_tick(&shiroi->z80, shiroi->z80_pins);
		if(shiroi->z80_pins & Z80_MREQ) {
			uint16_t addr = Z80_GET_ADDR(shiroi->z80_pins);
			if(shiroi->z80_pins & Z80_RD) {
				uint8_t data = shiroi->ram[addr];
				Z80_SET_DATA(shiroi->z80_pins, data);
			} else if(shiroi->z80_pins & Z80_WR) {
				if(addr >= 0x8000) {
					uint8_t data = Z80_GET_DATA(shiroi->z80_pins);
					shiroi->ram[addr] = data;
				}
			}
		} else if(shiroi->z80_pins & Z80_IORQ) {
			uint16_t io = Z80_GET_ADDR(shiroi->z80_pins);
			uint16_t addr = io & 0xff;
			uint16_t data = (io >> 8) & 0xff;

			if(shiroi->z80_pins & Z80_M1) {
			} else {
				shiroi_video_mk_i(shiroi);
				shiroi_video_mk_ii(shiroi);
				shiroi_sound_mk_i(shiroi);
				shiroi_math_mk_i(shiroi);
				shiroi_text_mk_i(shiroi);
			}
		}

		shiroi_video_mk_i_tick(shiroi);
		shiroi_video_mk_ii_tick(shiroi);
		shiroi_sound_mk_i_tick(shiroi);
		shiroi_math_mk_i_tick(shiroi);
		shiroi_text_mk_i_tick(shiroi);
	}
}
