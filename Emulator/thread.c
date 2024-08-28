/* $Id$ */

#ifdef __MINGW32__
#include <process.h>
#include <windows.h>
HANDLE thr;
#else
#include <pthread.h>
pthread_t th;
#endif

#include "shiroi.h"

extern shiroi_t shiroi;

#ifdef __MINGW32__
unsigned int WINAPI loop(LPVOID arg) {
#else
void* loop(void* arg) {
#endif
	shiroi_loop(&shiroi);
#ifdef __MINGW32__
	return 0;
#else
	return NULL;
#endif
}

void thread_start(void) {
#ifdef __MINGW32__
	thr = (HANDLE)_beginthreadex(NULL, 0, loop, NULL, 0, NULL);
#else
	pthread_create(&th, NULL, loop, NULL);
#endif
}

void thread_end(void) {
#ifdef __MINGW32__
	WaitForSingleObject(thr, INFINITE);
#else
	pthread_join(th, NULL);
#endif
}
