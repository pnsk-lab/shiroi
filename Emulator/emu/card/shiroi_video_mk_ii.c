/* $Id$ */
#include "shiroi_video_mk_ii.h"

#include "shiroi.h"

#include <stdlib.h>

uint64_t shiroi_video_mk_ii_cb(uint64_t pins, void* data) {
	shiroi_card_t* card = (shiroi_card_t*)data;
	MC6847_SET_DATA(pins, card->video.vram[MC6847_GET_ADDR(pins)]);
	return pins;
}

void shiroi_video_mk_ii_install(shiroi_t* shiroi, int slot) {
	shiroi->cards[slot].type = SHIROI_VIDEO_MARK_II;
	shiroi->cards[slot].videoptr = &shiroi->cards[slot].video;

	shiroi->cards[slot].video.width = MC6847_IMAGE_WIDTH + MC6847_BORDER_PIXELS * 2;
	shiroi->cards[slot].video.height = MC6847_FRAMEBUFFER_HEIGHT;
	shiroi->cards[slot].video.tick = 0;

	mc6847_desc_t desc;
	desc.tick_hz = 3 * 1024 * 1024;
	desc.framebuffer.ptr = malloc(MC6847_FRAMEBUFFER_SIZE_BYTES);
	desc.framebuffer.size = MC6847_FRAMEBUFFER_SIZE_BYTES;
	desc.fetch_cb = shiroi_video_mk_ii_cb;
	desc.user_data = &shiroi->cards[slot];

	mc6847_init(&shiroi->cards[slot].video.mc6847, &desc);

	int x, y;
	for(y = 0; y < shiroi->cards[slot].video.height; y++) {
		for(x = 0; x < shiroi->cards[slot].video.width; x++) {
			shiroi->cards[slot].video.fb[y * shiroi->cards[slot].video.width + x] = 0;
		}
	}
}

void shiroi_video_mk_ii_reset(shiroi_t* shiroi, int slot) {
	mc6847_reset(&shiroi->cards[slot].video.mc6847);
	shiroi->cards[slot].video.tick = 0;
}

void shiroi_video_mk_ii(shiroi_t* shiroi) {
	int i;

	uint16_t io = Z80_GET_ADDR(shiroi->z80_pins);
	uint16_t addr = io & 0xff;
	uint16_t data = (io >> 8) & 0xff;

	for(i = 0; i < 256 / SHIROI_IO_PORTS; i++) {
		if(shiroi->cards[i].type == SHIROI_VIDEO_MARK_II) {
			if(shiroi->z80_pins & Z80_RD) {
				/* I/O Read */
				if(addr == 1) {
					Z80_SET_DATA(shiroi->z80_pins, shiroi->cards[i].video.vram[shiroi->cards[i].video.vram_addr++]);
				} else if(addr == 2) {
					Z80_SET_DATA(shiroi->z80_pins, shiroi->cards[i].type);
				}
			} else if(shiroi->z80_pins & Z80_WR) {
				/* I/O Write */
				if(addr == 0) {
					shiroi->cards[i].video.vram_addr >>= 8;
					shiroi->cards[i].video.vram_addr |= data << 8;
				} else if(addr == 1) {
					shiroi->cards[i].video.vram[shiroi->cards[i].video.vram_addr++] = data;
				}
			}
		}
	}
}

void shiroi_video_mk_ii_tick(shiroi_t* shiroi) {
	int i, y, x;
	for(i = 0; i < 256 / SHIROI_IO_PORTS; i++) {
		if(shiroi->cards[i].type == SHIROI_VIDEO_MARK_II) {
			shiroi->cards[i].video.tick++;
			mc6847_tick(&shiroi->cards[i].video.mc6847, shiroi->cards[i].video.mc6847.pins);
			if(shiroi->cards[i].video.tick == 16384) {
				shiroi->cards[i].video.tick = 0;
				for(y = 0; y < MC6847_FRAMEBUFFER_HEIGHT; y++) {
					for(x = 0; x < MC6847_FRAMEBUFFER_WIDTH; x++) {
						if(x < shiroi->cards[i].video.width) {
							uint32_t c = shiroi->cards[i].video.mc6847.hwcolors[shiroi->cards[i].video.mc6847.fb[y * MC6847_FRAMEBUFFER_WIDTH + x] & 0xf];
							shiroi->cards[i].video.fb[y * shiroi->cards[i].video.width + x] = ((c & 0xffffff) << 8) | ((c & 0xff000000) >> 24);
						}
					}
				}
			}
		}
	}
}
