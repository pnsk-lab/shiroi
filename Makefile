# $Id$

PWD = `pwd`
PLATFORM = generic

FLAGS= PWD=$(PWD) PLATFORM=$(PLATFORM)

include Platform/$(PLATFORM).mk

.PHONY: all fetch clean emulator rom ./Emulator ./ROM format

all: emulator rom

fetch:
	wget -O ROM/basic.c http://svn.nishi.boats/repo/krakow/trunk/BASIC/basic.c

emulator: ./Emulator
rom: ./ROM

./Emulator::
	$(MAKE) -C $@ $(FLAGS)

./ROM::
	$(MAKE) -C $@ $(FLAGS)

format:
	clang-format --verbose -i `find Emulator -name "*.c" -or -name "*.h"` ROM/basic.c

clean:
	$(MAKE) -C ./Emulator $(FLAGS) clean
	$(MAKE) -C ./ROM $(FLAGS) clean
