#include <stdbool.h>
#include <raylib.h>

extern unsigned char bcmx_thread_ctl;											// control byte
extern bool bcmx_thread_lock;													// thread lock

/* control background music thread */
void bcmx_ctl(unsigned char ctr) {
	bcmx_thread_lock = true;
	bcmx_thread_ctl = ctr;
	while(bcmx_thread_lock)
		WaitTime(0.04f);
}
