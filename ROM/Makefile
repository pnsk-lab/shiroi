# $Id$

.PHONY: all clean
.SUFFIXES: .c .rel

OBJS = shiroi.rel basic.rel io.rel char.rel mem.rel
OBJS += dri/math.rel dri/text.rel dri/video.rel dri/sound.rel dri/debug.rel

all: shiroi.rom

shiroi.rom: shiroi.bin font.bin
	cat shiroi.bin font.bin > shiroi.rom

shiroi.bin: Discard/shiroi.ihx
	mkdir -p Discard
	makebin -s 22528 Discard/shiroi.ihx shiroi.bin

font.bin: Discard/font.ihx
	mkdir -p Discard
	makebin -s 2048 Discard/font.ihx font.bin

Discard/shiroi.ihx: $(OBJS)
	mkdir -p Discard
	sdcc -mz80 -o $@ --data-loc 0x8000 --idata-loc 0x8000 --code-loc 0 --stack-loc 0x9000 --no-std-crt0 --nostdinc --nostdlib $(OBJS)

io.rel: io.asm
	mkdir -p Discard
	sdasz80 -o io.rel io.asm
	rm -f "`echo $< | sed -E 's/\.asm$$/.sym/g'`" "`echo $< | sed -E 's/\.asm$$/.lst/g'`"

.c.rel:
	mkdir -p Discard
	sdcc -DPLATFORM_SHIROI -DONLY_VDP -c -mz80 -o $@ --no-std-crt0 --nostdinc --nostdlib $<
	rm -f "`echo $< | sed -E 's/\.c$$/.asm/g'`" "`echo $< | sed -E 's/\.c$$/.sym/g'`" "`echo $< | sed -E 's/\.c$$/.lst/g'`"

Discard/font.ihx: font.asm
	mkdir -p Discard
	sdasz80 -o Discard/font.rel font.asm
	sdldz80 -i $@ Discard/font.rel

clean:
	rm -rf Discard *.o *.bin *.rom *.rel */*.rel basic.c
