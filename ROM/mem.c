/* $Id$ */

#include "mem.h"

void memcpy(void* dest, void* src, int len){
	unsigned char* d = (unsigned char*)dest;
	unsigned char* s = (unsigned char*)src;
	int i;
	for(i = 0; i < len; i++) d[i] = s[i];
}
