#include "kernel.h"

PCB idle, *current = &idle, threadA, threadB;

void
schedule(void) {
	/* implement process/thread schedule here */
	if(current == &idle || current == &threadB)
		current = &threadA;
	else if(current == &threadA)
		current = &threadB;
	else
		assert(0); //should never been here
}
