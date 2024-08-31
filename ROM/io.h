/* $Id$ */

#ifndef __IO_H__
#define __IO_H__

#if !((SDCC_VERSION_MAJOR == 4 && SDCC_VERSION_MINOR == 1 && SDCC_VERSION_PATCH == 11) || (SDCC_VERSION_MAJOR > 4) || (SDCC_VERSION_MAJOR == 4 && SDCC_VERSION_MINOR > 1) || (SDCC_VERSION_MAJOR == 4 && SDCC_VERSION_MINOR == 1 && SDCC_VERSION_PATCH > 11))
#define __sdcccall(x)
#endif

unsigned char inp(unsigned char) __sdcccall(0);
void outp(unsigned char, unsigned char) __sdcccall(0);

#endif
