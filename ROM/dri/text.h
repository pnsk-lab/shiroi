/* $Id$ */

#ifndef __TEXT_H__
#define __TEXT_H__

void text_init(void);
void text_card(int, int);

void clear(void);
void putchar(char);
void putstr(char*);
void putnum(int);
void print_ptr(void*);
char agetch(void);
char getch(void);
void cursor(void);
void killcursor(void);

#endif
