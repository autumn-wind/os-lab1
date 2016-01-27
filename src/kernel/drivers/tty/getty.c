#include "kernel.h"
#include "tty.h"
#include "hal.h"

static int tty_idx = 1;

void
getty(void) {
	char name[] = "tty0";
	lock();
	name[3] += (tty_idx ++);
	unlock();

	while(1) {
		/* Insert code here to do these:
		 * 1. read key input from ttyd to buf (use dev_read())
		 * 2. convert all small letters in buf into capitcal letters
		 * 3. write the result on screen (use dev_write())
		 */
		char buf[CBUF_SZ];
		int n = dev_read(name, current->pid, buf, 0, CBUF_SZ);
		int i;
		for(i = 0; i < n; i++){
			if(buf[i] >= 'a' && buf[i] <= 'z')
				buf[i] += 'A' - 'a';
		}
		dev_write(name, current->pid, buf, 0, n);
	}
}

void
init_getty(void) {
	int i;
	for(i = 0; i < NR_TTY; i ++) {
		wakeup(create_kthread(getty));
	}
}


