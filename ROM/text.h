/* $Id$ */

#ifndef __TEXT_H__
#define __TEXT_H__

void text_init(void);
void text_card(int t, int port);

void clear(void);
void putchar(char);
void putstr(char*);
void scroll_y(void);
void print_ptr(void*);
char getch(void);
void cursor(void);
void killcursor(void);

#endif
