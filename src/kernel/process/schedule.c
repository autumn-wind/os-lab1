#include "kernel.h"

PCB idle, *current = &idle, pcb[7];

void
schedule(void) {
	/* implement process/thread schedule here */
		if (current == &idle)
			current = &pcb[0];
		else if(current == &pcb[6])
			current = current - 6;
		else
			current += 1;
}
