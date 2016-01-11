#include "kernel.h"
extern PCB pcb[3];
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

void sleep(void);
void wakeup(PCB *p);

void A () { 
    int x = 0;
    while(1) {
        if(x % 100000 == 0) {
            printk("a");
            wakeup(&pcb[1]);
            sleep();
        }
        x ++;
    }
}
void B () { 
    int x = 0;
    while(1) {
        if(x % 100000 == 0) {
            printk("b");
            wakeup(&pcb[2]);
            sleep();
        }
        x ++;
    }
}
void C () { 
    int x = 0;
    while(1) {
        if(x % 100000 == 0) {
            printk("c");
            wakeup(&pcb[3]);
            sleep();
        }
        x ++;
    }
}
void D () { 
    int x = 0;
    while(1) {
        if(x % 100000 == 0) {
            printk("d");
            wakeup(&ocb[0]);
            sleep();
        }
        x ++;
    }
}

void
init_proc() {
    create_kthread(A, &pcb[0]);
    create_kthread(A, &pcb[1]);
    create_kthread(A, &pcb[2]);
    create_kthread(A, &pcb[3]);
}

