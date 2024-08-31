/* $Id$ */

#ifndef __IO_H__
#define __IO_H__

#if __SDCCCALL == 0
#define __sdcccall(x)
#endif

unsigned char inp(unsigned char) __sdcccall(0);
void outp(unsigned char, unsigned char) __sdcccall(0);

#endif
