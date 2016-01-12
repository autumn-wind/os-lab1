#include "kernel.h"
extern PCB pcb[4], idle;
extern ListHead ready, block, free;

void sleep(void);
void wakeup(PCB *p);

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
            wakeup(&pcb[0]);
            sleep();
        }
        x ++;
    }
}

void sleep(){
	list_del(&current->list);
	list_add_after(&block, &current->list);
	asm volatile("int $0x80");
}

void wakeup(PCB *p){
	list_del(&p->list);
	list_add_after(&ready, &p->list);
}

void
init_proc() {
    create_kthread(A, &pcb[0]);
    create_kthread(B, &pcb[1]);
    create_kthread(C, &pcb[2]);
    create_kthread(D, &pcb[3]);
	list_init(&ready);
	list_init(&block);
	list_init(&free);
	list_add_after(&ready, &idle.list);
	list_add_after(&ready, &pcb[0].list);
	list_add_after(&block, &pcb[1].list);
	list_add_after(&block, &pcb[2].list);
	list_add_after(&block, &pcb[3].list);
}

