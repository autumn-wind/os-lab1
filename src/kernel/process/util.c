#include "kernel.h"
extern PCB pcb[4], idle;
extern ListHead ready, block, free;
extern uint32_t pnum;

void sleep(void);
void wakeup(PCB *p);

PCB*
create_kthread(void *fun) {
	PCB *p = &pcb[pnum++];
	TrapFrame *frame = (TrapFrame *)(p->kstack + KSTACK_SIZE) - 1; 
	frame->ds = 0x10;
	frame->es = 0x10;
	frame->fs = 0x10;
	frame->gs = 0x10;
	frame->cs = 8;
	frame->eip = (uint32_t)fun;
	frame->eflags |= 0x200;
	p->tf = frame;
	return p;
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
	list_add_before(&block, &current->list);
	asm volatile("int $0x80");
}

void wakeup(PCB *p){
	list_del(&p->list);
	list_add_before(&idle.list, &p->list);
}

void
init_proc() {
    create_kthread(A);
    create_kthread(B);
    create_kthread(C);
    create_kthread(D);
	list_init(&ready);
	list_init(&block);
	list_init(&free);
	list_add_before(&ready, &idle.list);
	list_add_before(&idle.list, &pcb[0].list);
	list_add_before(&block, &pcb[1].list);
	list_add_before(&block, &pcb[2].list);
	list_add_before(&block, &pcb[3].list);
}

