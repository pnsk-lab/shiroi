# $Id$

PWD = `pwd`
PLATFORM = generic

FLAGS= PWD=$(PWD) PLATFORM=$(PLATFORM)

include Platform/$(PLATFORM).mk

.PHONY: all clean emulator rom ./Emulator ./ROM format

all: emulator rom

emulator: ./Emulator
rom: ./ROM

./Emulator::
	$(MAKE) -C $@ $(FLAGS)

./ROM::
	$(MAKE) -C $@ $(FLAGS)

format:
	clang-format --verbose -i `find . -name "*.c" -or -name "*.h"`

clean:
	$(MAKE) -C ./Emulator $(FLAGS) clean
	$(MAKE) -C ./ROM $(FLAGS) clean
