#!/bin/sh
# $Id$
avr-gcc -DPLATFORM_ARDUINO -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o basic.o ROM/basic.c || exit 1
avr-gcc -mmcu=atmega328p basic.o -o basic || exit 1
rm basic.o
avr-objcopy -O ihex -R .eeprom basic basic.hex || exit 1
rm basic
if [ "x$DEVICE" = "x" ]; then
	echo -n "Device? "
	read DEVICE
fi
avrdude -F -V -c arduino -p ATMEGA328P -P $DEVICE -b 115200 -U flash:w:basic.hex
