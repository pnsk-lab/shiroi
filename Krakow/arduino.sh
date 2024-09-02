#!/bin/sh
# $Id$

avr-gcc -DPLATFORM_ARDUINO -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o arduino.o ../ROM/basic.c || exit 1
avr-gcc -mmcu=atmega328p arduino.o -o arduino || exit 1
rm arduino.o
avr-objcopy -O ihex -R .eeprom arduino arduino.hex || exit 1
rm arduino
if [ "x$DEVICE" = "x" ]; then
	echo -n "Device? "
	read DEVICE
fi
avrdude -F -V -c arduino -p ATMEGA328P -P $DEVICE -b 115200 -U flash:w:arduino.hex
