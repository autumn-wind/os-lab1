#include "kernel.h"

PCB idle, pcb[MAXPCB_NUM], *current = &idle;
ListHead ready;
pid_t pnum = 0;

void
schedule(void) {
	/* implement process/thread schedule here */
	assert(!list_empty(&ready));
	if(current->list.prev == &ready){
		list_del(&current->list);
		list_add_before(&idle.list, &current->list);
	}
	current = listhead_to_pcb(ready.next);
}

PCB* fetch_pcb(pid_t pid){
	return &pcb[pid];
}

