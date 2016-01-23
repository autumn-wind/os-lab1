#include "kernel.h"

PCB idle, pcb[MAXPCB_NUM], *current = &idle;
ListHead ready;
pid_t pnum = 0;

void
schedule(void) {
	/* implement process/thread schedule here */
	if(list_empty(&ready))
		return;
	current = listhead_to_pcb(ready.next);
}
