#include "kernel.h"
extern PCB pcb[7];
//extern TrapFrame;
PCB*
create_kthread(void *fun, uint32_t ch, PCB *pcb) {
	TrapFrame *frame = (TrapFrame *)(pcb->kstack + KSTACK_SIZE) - 1; 
	frame->ds = 0x10;
	frame->es = 0x10;
	frame->fs = 0x10;
	frame->gs = 0x10;
	frame->cs = 8;
	frame->eip = (uint32_t)fun;
	frame->eflags |= 0x200;
	frame->args = ch;
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

void print_ch (int ch) { 
    int x = 0;
    while(1) {
        if(x % 30000000 == 0) {printk("%c", ch);}
        x ++;
    }
}

void
init_proc() {
	int i = 0;
	for(i = 0; i < 7; i ++) {
    create_kthread(print_ch, 'a' + i, &pcb[i]);
	}
}

