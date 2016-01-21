#include "kernel.h"

PCB idle, pcb[MAXPCB_NUM], *current = &idle;
ListHead ready;
uint32_t pnum = 0;

void
schedule(void) {
	/* implement process/thread schedule here */
	if(list_empty(&ready))
		return;
	/*
	 *list_foreach(ptr, &ready)
	 *    if(ptr == &current->list)
	 *        break;
	 *if(ptr != &ready){
	 *    if(ptr->next != &ready)	ptr = ptr->next;
	 *    else ptr = ready.next;	
	 *}else{
	 *    ptr = ready.next;
	 *}
	 */
	/*current = (PCB *)(char *)(ptr + 1) - 1;*/
	current = listhead_to_pcb(ready.next);
}
