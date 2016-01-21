#include "kernel.h"

PCB idle, pcb[4], *current = &idle;
ListHead ready, block, free;
uint32_t pnum = 0;

void
schedule(void) {
	/* implement process/thread schedule here */
	if(list_empty(&ready))
		return;
	ListHead *ptr = ready.next;
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
	current = (PCB *)((char *)ptr - KSTACK_SIZE - sizeof(void *));
}
