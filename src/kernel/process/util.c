#include "kernel.h"
extern PCB threadA, threadB;
//extern TrapFrame;
PCB*
create_kthread(void *fun, PCB *pcb) {
	TrapFrame *frame = (TrapFrame *)(pcb->kstack + KSTACK_SIZE) - 1; 
	frame->ds = 0x10;
	frame->es = 0x10;
	frame->fs = 0x10;
	frame->gs = 0x10;
	frame->cs = 8;
	frame->eip = (uint32_t)fun;
	frame->eflags |= 0x200;
	pcb->tf = frame;
	return NULL;
}

void A () { 
    int x = 0;
    while(1) {
        if(x % 50000000 == 0) {printk("a");}
        x ++;
    }
}
void B () { 
    int x = 0;
    while(1) {
        if(x % 50000000 == 0) {printk("b");}
        x ++;
    }
}

void
init_proc() {
	create_kthread((void *)A, &threadA);
	create_kthread((void *)B, &threadB);
}

