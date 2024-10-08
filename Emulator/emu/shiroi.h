/* $Id$ */

#ifndef __SHIROI_H__
#define __SHIROI_H__

#include "chips_common.h"
#include "z80.h"
#include "ay38910.h"
#include "tms9918.h"
#include "mc6847.h"

#include <stdint.h>
#include <stdbool.h>

#define SHIROI_IO_PORTS 3

enum SHIROI_CARD {
	SHIROI_VIDEO = 0x00,
	SHIROI_VIDEO_MARK_I,
	SHIROI_VIDEO_MARK_II,
	SHIROI_SOUND = 0x10,
	SHIROI_SOUND_MARK_I,
	SHIROI_SYSTEM = 0x20,
	SHIROI_MATH_MARK_I,
	SHIROI_TEXT_MARK_I,
	SHIROI_DEBUG,
	SHIROI_ROMCARD_MARK_I
};

typedef struct {
	uint32_t fb[800 * 600];
	union {
		VrEmuTms9918* vdp;
		mc6847_t mc6847;
	};
	uint16_t vram_addr;
	uint8_t vram[64 * 1024];
	int width;
	int height;
	int tick;
} shiroi_video_t;

typedef struct {
	union {
		ay38910_t psg;
	};
	int tick;
} shiroi_sound_t;

typedef struct {
	union {
		void* am9511;
	};
	int tick;
} shiroi_math_t;

typedef struct {
	unsigned char key;
	bool caps;
	int tick;
} shiroi_text_t;

typedef struct {
	unsigned char latch[4];
	int latch_addr;
	int tick;
} shiroi_debug_t;

typedef struct {
	unsigned char* data;
	int latch_addr;
} shiroi_romcard_t;

typedef struct {
	union {
		shiroi_video_t video;
		shiroi_sound_t sound;
		shiroi_math_t math;
		shiroi_text_t text;
		shiroi_debug_t debug;
		shiroi_romcard_t romcard;
	};
	shiroi_video_t* videoptr;
	shiroi_sound_t* soundptr;
	shiroi_math_t* mathptr;
	shiroi_text_t* textptr;
	shiroi_debug_t* debugptr;
	shiroi_romcard_t* romcardptr;
	int type;
} shiroi_card_t;

typedef struct {
	uint8_t ram[64 * 1024];
	uint64_t z80_pins;
	z80_t z80;
	bool stop;
	void (*play_audio)(void* buffer, unsigned int frames);
	shiroi_card_t cards[256 / 3];
	bool reset;
} shiroi_t;

void shiroi_init(shiroi_t* shiroi);
void shiroi_init_cards(shiroi_t* shiroi);
void shiroi_loop(shiroi_t* shiroi);
void shiroi_install(shiroi_t* shiroi, int slot, int card);
shiroi_card_t* shiroi_get_video_card(shiroi_t* shiroi);
shiroi_card_t* shiroi_get_sound_card(shiroi_t* shiroi);
shiroi_card_t* shiroi_get_math_card(shiroi_t* shiroi);
shiroi_card_t* shiroi_get_text_card(shiroi_t* shiroi);
shiroi_card_t* shiroi_get_debug_card(shiroi_t* shiroi);
shiroi_card_t* shiroi_get_romcard_card(shiroi_t* shiroi);

#endif
