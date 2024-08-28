# $Id$

.PHONY: all clean

all: shiroi.rom

shiroi.rom: shiroi.bin font.bin
	cat shiroi.bin font.bin > shiroi.rom

shiroi.bin: Discard/shiroi.ihx
	makebin -s 8192 Discard/shiroi.ihx shiroi.bin

font.bin: Discard/font.ihx
	makebin -s 8192 Discard/font.ihx font.bin

Discard/shiroi.ihx: shiroi.c
	mkdir -p Discard
	sdcc -mz80 -o Discard/shiroi.ihx --data-loc 0x1000 --code-loc 0 --no-std-crt0 --nostdinc --nostdlib shiroi.c

Discard/font.ihx: font.asm
	mkdir -p Discard
	sdasz80 -o Discard/font.rel font.asm
	sdldz80 -i $@ Discard/font.rel

clean:
	rm -rf Discard *.o *.bin *.rom
