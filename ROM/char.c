/* $Id$ */

#include "char.h"

char toupper(char c){
	if('a' <= c && c <= 'z'){
		return c - 'a' + 'A';
	}
	return c;
}

int strlen(const char* str){
	int i;
	for(i = 0; str[i] != 0; i++);
	return i;
}
