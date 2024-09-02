/* $Id$ */

#if defined(PLATFORM_SHIROI)

#include "dri/text.h"
#include "dri/video.h"
#include "dri/math.h"

#include "mem.h"
#include "char.h"
#define PLATFORM "Shiroi"
#define NEWLINE "\r\n"
#define BREAKKEY

#elif defined(PLATFORM_UNIX) || defined(PLATFORM_WINDOWS)

#define mull(x,y) ((x)*(y))
#define divl(x,y) ((x)/(y))
#define killcursor(x)
#define cursor(x)
#define strnum atoi
#define PLATFORM "Unix"
#define NEWLINE "\n"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#ifdef __MINGW32__
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#endif

char agetch(void){
#ifdef __MINGW32__
	int c;
rescan:
	c = _getch();
	if(c == '\r') return '\n';
	if(c == '\n') goto rescan;
	return c;
#else
	int c = getchar();
	if(c == EOF) return -1;
	if(c == '\r') return '\n';
#endif
	return c;
}

bool strcaseequ(const char* a, const char* b){
	return strcasecmp(a, b) == 0;
}

void change_color(int a){
}

void putnum(int n){
	printf("%d", n);
	fflush(stdout);
}

void putstr(const char* n){
	printf("%s", n);
	fflush(stdout);
}

void clear(void){
	printf("\x1b[2J\x1b[1;1H");
	fflush(stdout);
}

void basic(void);

int main(){
#ifdef __MINGW32__
	HANDLE winstdout = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD mode = 0;
	GetConsoleMode(winstdout, &mode);
	const DWORD origmode = mode;
	mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(winstdout, mode);
#else
	struct termios old, new;
	tcgetattr(0, &old);
	new = old;
	new.c_lflag &= ~(ECHO | ICANON);
	tcsetattr(0, TCSANOW, &new);
#endif
	basic();
#ifdef __MINGW32__
	SetConsoleMode(winstdout, origmode);
#else
	tcsetattr(0, TCSANOW, &old);
#endif
}

#else
#error "Define PLATFORM_*"
#endif

#define VERSION "0.0"
#define LINE_BUFFER_SIZE 512
#define BUFFER_SIZE (1024 * 24)

unsigned char basicbuffer[BUFFER_SIZE];
char linebuf[LINE_BUFFER_SIZE];

int pexpr(char* expr, char* buffer, int* number){
	char ownbuf[128];
	int i;
	for(i = 0; expr[i] != 0; i++) ownbuf[i] = expr[i];
	ownbuf[i] = 0;
	int start = 0;
	int br = 0;
	int result = 0;
	int stack[128];
	int sp = 0;
	char put = 0;
	for(i = 0; i < 128; i++) stack[i] = 0;
	for(i = 0;; i++){
		if(ownbuf[i] == 0){
			break;
		}else if('0' <= ownbuf[i] && ownbuf[i] <= '9'){
			stack[sp] *= 10;
			stack[sp] += ownbuf[i] - '0';
			put = 1;
		}else if(ownbuf[i] == '+' || ownbuf[i] == '-' || ownbuf[i] == '*' || ownbuf[i] == '/'){
			put = 0;
			if(sp < 2){
				return -1;
			}else{
				int top = stack[--sp];
				int bottom = stack[--sp];
				int value = 0;
				if(ownbuf[i] == '+'){
					value = top + bottom;
				}else if(ownbuf[i] == '-'){
					value = bottom + top;
				}else if(ownbuf[i] == '*'){
					value = mull(top, bottom);
				}else if(ownbuf[i] == '/'){
					value = divl(bottom, top);
				}
				stack[sp++] = value;
			}
			stack[sp] = 0;
		}else if(ownbuf[i] == ' ' && put == 1){
			stack[++sp] = 0;
		}
	}
	result = stack[0];
	*number = result;
	return 1;
}

/* arr gets sorted, arr2 index gets replaced */
void sort(int* arr, int* arr2, int size){
	int i;
redo:
	for(i = 1; i < size; i++){
		if(arr[i - 1] > arr[i]){
			int tmp = arr[i];
			arr[i] = arr[i - 1];
			arr[i - 1] = tmp;
			tmp = arr2[i];
			arr2[i] = arr2[i - 1];
			arr2[i - 1] = tmp;
		}
	}
	for(i = 1; i < size; i++){
		if(arr[i - 1] > arr[i]){
			goto redo;
		}
	}
}

int run(char* cmd, int linenum, char num, int* lgoto){
#ifdef BREAKKEY
	if(agetch() == 1) return -1;
#endif
	char line[LINE_BUFFER_SIZE];
	char rcmd[32];
	int i;
	if(lgoto != 0) *lgoto = 0;
	for(i = 0; cmd[i] != 0; i++) line[i] = cmd[i];
	line[i] = 0;
	rcmd[0] = 0;
	int incr = 0;
	for(i = 0;; i++){
		if(line[i] == ' ' || line[i] == '\t' || line[i] == 0 || line[i] == '"'){
			break;
		}else{
			rcmd[incr++] = line[i];
			if(incr == 32){
				putstr("! Command too long");
				if(linenum != -1){
					putstr(" in line ");
					putnum(linenum);
				}
				putstr(NEWLINE);
				return 1;
			}
			rcmd[incr] = 0;
		}
	}
	char* arg = line + 1 + strlen(rcmd);
	if(strcaseequ(rcmd, "COLOR")){
		int argc = 0;
		char* farg = 0;
		char* sarg = 0;
		if(arg[0] != 0) argc++;
		for(i = 0; arg[i] != 0; i++){
			if(arg[i] == ','){
				arg[i] = 0;
				farg = arg;
				sarg = arg + i + 1;
				for(; *sarg != 0 && (*sarg == '\t' || *sarg == ' '); sarg++);
				argc++;
			}
		}
		if(argc != 2){
			putstr("! Invalid argument");
			if(linenum != -1){
				putstr(" in line ");
				putnum(linenum);
			}
			putstr(NEWLINE);
			return 1;
		}
		int bgcolor = 0;
		int fgcolor = 0;
		int ret0 = pexpr(farg, 0, &bgcolor);
		int ret1 = pexpr(sarg, 0, &fgcolor);
		if(ret0 == 0){
			putstr("! Invalid argument");
			if(linenum != -1){
				putstr(" in line ");
				putnum(linenum);
			}
			putstr(NEWLINE);
			return 1;
		}else if(ret0 == -1){
			putstr("! Syntax error");
			if(linenum != -1){
				putstr(" in line ");
				putnum(linenum);
			}
			putstr(NEWLINE);
			return 1;
		}
		if(ret1 == 1){
			change_color((fgcolor << 4) | bgcolor);
		}else if(ret1 == 0){
			putstr("! Invalid argument");
			if(linenum != -1){
				putstr(" in line ");
				putnum(linenum);
			}
			putstr(NEWLINE);
			return 1;
		}else if(ret1 == -1){
			putstr("! Syntax error");
			if(linenum != -1){
				putstr(" in line ");
				putnum(linenum);
			}
			putstr(NEWLINE);
			return 1;
		}
	}else if(num == 0 && strcaseequ(rcmd, "LIST")){
		int addr = BUFFER_SIZE - 1;
		int saddr = 0;
		int lbuf[1024];
		int shift[1024];
		int cnt = 0;
		while(1){
			unsigned long ln = 0;
			for(i = 0; i < 4; i++){
				ln >>= 8;
				ln |= (unsigned long)basicbuffer[addr--] << 24;
			}
			if(ln == 0) break;
			lbuf[cnt] = ln;
			shift[cnt++] = saddr;
			saddr += strlen(basicbuffer + saddr) + 1;
		}
		sort(lbuf, shift, cnt);
		int i;
		for(i = 0; i < cnt; i++){
			putnum(lbuf[i]);
			putstr(" ");
			putstr(basicbuffer + shift[i]);
			putstr(NEWLINE);
		}
	}else if(num == 1 && strcaseequ(rcmd, "GOTO")){
		int ret = pexpr(arg, 0, lgoto);
		if(ret == 0){
			putstr("! Invalid argument");
			if(linenum != -1){
				putstr(" in line ");
				putnum(linenum);
			}
			putstr(NEWLINE);
			return 1;
		}else if(ret == -1){
			putstr("! Syntax error");
			if(linenum != -1){
				putstr(" in line ");
				putnum(linenum);
			}
			putstr(NEWLINE);	
			return 1;
		}
	}else if(num == 0 && strcaseequ(rcmd, "RUN")){
		int addr = BUFFER_SIZE - 1;
		int saddr = 0;
		int lbuf[1024];
		int shift[1024];
		int cnt = 0;
		int gt;
		while(1){
			unsigned long ln = 0;
			for(i = 0; i < 4; i++){
				ln >>= 8;
				ln |= (unsigned long)basicbuffer[addr--] << 24;
			}
			if(ln == 0) break;
			lbuf[cnt] = ln;
			shift[cnt++] = saddr;
			saddr += strlen(basicbuffer + saddr) + 1;
		}
		sort(lbuf, shift, cnt);
		int i;
		for(i = 0; i < cnt; i++){
			int ret = run(basicbuffer + shift[i], lbuf[i], 1, &gt);
			if(ret != 0) return ret;
			if(gt != 0){
				char found = 0;
				for(i = 0; i < cnt; i++){
					if(lbuf[i] == gt){
						found = 1;
						break;
					}
				}
				if(found){
					i--;
				}else{
					putstr("! GOTO no such line");
					if(linenum != -1){
						putstr(" in line ");
						putnum(linenum);
					}
					putstr(NEWLINE);
					return 1;
				}
			}
		}
	}else{
		putstr("! Unknown command");
		if(linenum != -1){
			putstr(" in line ");
			putnum(linenum);
		}
		putstr(NEWLINE);
		return 1;
	}
	return 0;
}

int execute(int linenum, char* cmd, char num){
	int i;
	char line[LINE_BUFFER_SIZE];
	int incr = 0;
	char dq = 0;
	for(i = 0; cmd[i] != 0; i++){
		if(cmd[i] == ' ' || cmd[i] == '\t'){
			if(!dq){
				for(; cmd[i] != 0 && (cmd[i] == ' ' || cmd[i] == '\t'); i++);
				i--;
			}
			line[incr++] = cmd[i];
		}else if(cmd[i] == '"'){
			line[incr++] = '"';
			dq = dq == 0 ? 1 : 0;
		}else{
			line[incr++] = dq == 0 ? toupper(cmd[i]) : cmd[i];
		}
	}
	line[incr] = 0;
	if(num == 0){
		int ret = run(line, -1, 0, 0);
		putstr("Ready\r\n");
		return ret;
	}else{
		int addr = BUFFER_SIZE - 1;
		int i;
		int shf = 0;
		int cnt = 0;
		while(1){
			unsigned long ln = 0;
			for(i = 0; i < 4; i++){
				ln >>= 8;
				ln |= (unsigned long)basicbuffer[addr--] << 24;
			}
			cnt++;
			if(ln == linenum) shf = cnt;
			if(shf != 0){
				addr += 4;
				for(i = 0; i < 4; i++){
					basicbuffer[addr] = basicbuffer[addr - 4 * shf];
					addr--;
				}
				addr += 4;
				ln = 0;
				for(i = 0; i < 4; i++){
					ln >>= 8;
					ln |= (unsigned long)basicbuffer[addr--] << 24;
				}
			}
			if(ln == 0) break;
		}
		if(line[0] != 0){
			addr += 4;
			for(i = 0; i < 4; i++){
				basicbuffer[addr--] = linenum & 0xff;
				linenum >>= 8;
			}
		}
		int len = 0;
		cnt = 0;
		while(1){
			int slen = strlen(basicbuffer + len);
			if(slen == 0) break;
			len += slen + 1;
			cnt++;
		}
		if(line[0] != 0){
			for(i = 0; line[i] != 0; i++){
				basicbuffer[len + i] = line[i];
			}
			basicbuffer[len + i] = 0;
			basicbuffer[len + i + 1] = 0;
		}
		if(shf != 0){
			cnt = 0;
			len = 0;
			while(1){
				int slen = strlen(basicbuffer + len);
				if(slen == 0) break;

				len += slen + 1;
				cnt++;

				if(cnt == shf){
					len -= slen + 1;
					int i;
					int nc = 0;
					for(i = len;; i++){
						basicbuffer[i] = basicbuffer[i + slen + 1];
						if(basicbuffer[i] == 0){
							nc++;
							if(nc == 2) break;
						}else{
							nc = 0;
						}
					}
					break;
				}
			}
		}
		return 0;
	}
}

void basic(void){
	int i;
	clear();

#ifdef SMALL
	putstr("Krakow BASIC  Ver. ");
	putstr(VERSION);
	putstr(NEWLINE);
	putstr(NEWLINE);
#else
	putstr(PLATFORM);
	putstr("   Krakow BASIC V");
	putstr(VERSION);
	putstr("\r\n");
	putstr("Copyright 2024 by: Nishi.\r\n");
	putstr("                   penguin2233.");
	putstr(NEWLINE);
	putstr(NEWLINE);
	putstr(" ");
#endif
	putnum(BUFFER_SIZE);
	putstr(" bytes free");
	putstr(NEWLINE);
	putstr(NEWLINE);

	for(i = 0; i < BUFFER_SIZE; i++){
		basicbuffer[i] = 0;
	}
	putstr("Ready");
	putstr(NEWLINE);

	cursor();
	linebuf[0] = 0;
	int lineind = 0;
	while(1){
		char c = agetch();
		if(c != 0) killcursor();
		if(c == 1){
			putstr("Break");
			putstr(NEWLINE);
			lineind = 0;
		}else if(c == '\n'){
			linebuf[lineind] = 0;
			putstr(NEWLINE);
			if(lineind == 0) goto skip;

			int i;
			char num = 1;

			char* cmd = linebuf;

			for(i = 0; linebuf[i] != 0; i++){
				if(linebuf[i] == ' '){
					linebuf[i] = 0;
					cmd = linebuf + i + 1;
					break;
				}else if(!('0' <= linebuf[i] && linebuf[i] <= '9')){
					num = 0;
					break;
				}
			}

			if(num == 1 && strnum(linebuf) == 0){
				putstr("! Line number 0 is illegal");
				putstr(NEWLINE);
			}else{
				int ret = execute(num == 1 ? strnum(linebuf) : -1, (num == 1 && cmd == linebuf) ? "" : cmd, num);
				if(ret == -1){
					putstr("! Break");
					putstr(NEWLINE);
				}
			}

skip:
			lineind = 0;
		}else if(c == 0x8){
			if(lineind > 0){
				putstr("\x08 \x08");
				lineind--;
			}
		}else if(c == -1){
			break;
		}else if(c != 0){
			putchar(c);
			linebuf[lineind++] = c;
		}
		cursor();
	}
}
