# $Id$

include $(PWD)/Platform/$(PLATFORM).mk

.PHONY: all clean
.SUFFIXES: .c .o

OBJS = main.o thread.o emu/shiroi.o
OBJS += emu/card/shiroi_video_mk_i.o emu/card/shiroi_video_mk_ii.o
OBJS += emu/card/shiroi_sound_mk_i.o
OBJS += emu/card/shiroi_math_mk_i.o emu/card/shiroi_text_mk_i.o emu/card/shiroi_debug.o
OBJS += emu/chips.o emu/tms9918.o emu/am9511.o emu/tms9918util.o emu/ova.o emu/floatcnv.o

all: shiroi$(EXEC)

shiroi$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

.c.o:
	$(CC) -I emu $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o */*.o */*/*.o *.exe shiroi
