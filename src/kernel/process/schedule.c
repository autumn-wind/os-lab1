#include "kernel.h"
inline void set_tss_esp0(uint32_t esp);

PCB idle, pcb[MAXPCB_NUM], *current = &idle;
ListHead ready, msg_pool, pcb_pool;
Msg msgs[MAXMSG_NUM];
uint8_t pid_pool[MAXPCB_NUM];

void
schedule(void) {
	/*printk("pid: %d\n", current->pid);*/
	/* implement process/thread schedule here */
	assert(!list_empty(&ready));
	if(current != &idle && current->list.prev == &ready){
		list_del(&current->list);
		list_add_before(&idle.list, &current->list);
	}
	current = list_entry(ready.next, PCB, list);
	/*printk("current->tf: %x\n", current->tf);*/
	set_tss_esp0((uint32_t)current->tf + (uint32_t)( (TrapFrame *)0 + 1 ));
	/*set_tss_esp0((uint32_t)current->tf);*/
	assert(current != &idle);
	write_cr3(&current->cr3);
}

PCB* fetch_pcb(pid_t pid){
	int i;
	for(i = 0; i < MAXPCB_NUM; ++i){
		if(pid == pcb[i].pid)
			return &pcb[i];
	}
	assert(0);
}

