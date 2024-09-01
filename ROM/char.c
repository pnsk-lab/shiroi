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

char strequ(const char* a, const char* b){
	if(strlen(a) != strlen(b)) return 0;
	int i;
	for(i = 0; a[i] != 0; i++){
		if(a[i] != b[i]) return 0;
	}
	return 1;
}

int strnum(const char* str){
	int i = 0;
	int num = 0;
	for(i = 0; str[i] != 0; i++){
		num = num * 10;
		num += str[i] - '0';
	}
	return num;
}

char strcaseequ(const char* a, const char* b){
	if(strlen(a) != strlen(b)) return 0;
	int i;
	for(i = 0; a[i] != 0; i++){
		if(toupper(a[i]) != toupper(b[i])) return 0;
	}
	return 1;
}
