#include "kernel.h"

PCB idle, pcb[3], *current = &idle;
ListHead ready, block, free;

void
schedule(void) {
	/* implement process/thread schedule here */
}
