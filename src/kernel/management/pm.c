#include "kernel.h"

static void pm(void);
TrapFrame* init_pcb(PCB *p, void *fun);

pid_t PM;

void init_pm(){
	PCB *p = create_kthread(pm);
	PM = p->pid;
	wakeup(p);
}

static void pm(void){
	uint32_t va, pa;
	unsigned char *i, *vdest;
	char buf[512];
	Msg m;
	lock();
	if(pnum >= MAXPCB_NUM){
		panic("no pcb for more thread");
	}
	PCB *p = &pcb[pnum];
	p->pid = pnum++;
	unlock();
	do_read(0, (uint8_t *)buf, 0, 512);

	/*int j;*/
	/*lock();*/
	/*for(j = 0; j < 512; j++){*/
		/*uint8_t c = buf[j];*/
		/*serial_printc(hexBoard[(c & 0xf0) >> 4]);*/
		/*serial_printc(hexBoard[(c & 0x0f)]);*/
		/*serial_printc(' ');*/
	/*}*/
	/*serial_printc('\n');*/
	/*unlock();*/
	struct ELFHeader *elf = (struct ELFHeader *)buf;
	struct ProgramHeader *ph, *eph;
	ph = (struct ProgramHeader*)((char *)elf + elf->phoff);
	eph = ph + elf->phnum;
	for(; ph < eph; ++ph){
		va = ph->vaddr;
		/*printk("va: %x\t, filesz: %x\t, memsz: %x\n", va, ph->filesz, ph->memsz);*/
		m.src = current->pid;
		m.type = NEW_PAGE;
		m.offset = va;
		m.len = ph->memsz;
		send(MM, &m);
		receive(MM, &m);
		pa = m.ret;
		/*printk("pa receiverd in pm: %x\n", pa);*/
		vdest = pa_to_va(pa);
		do_read(0, vdest, ph->off, ph->filesz);
		for(i = vdest + ph->filesz; i < vdest + ph->memsz; *i++ = 0);
	}
	init_pcb(p, (void *)(elf->entry));	
	p->cr3.val = 0;
	p->cr3.page_directory_base = (10 * PD_SIZE) >> 12;
	/*printk("user cr3: %x\n", p->cr3.page_directory_base);*/
	m.src = current->pid;
	m.type = SHARE_KERNEL_PAGE;
	m.offset = KOFFSET;
	m.len = KMEM;
	send(MM, &m);
	receive(MM, &m);
	wakeup(p);
	while(1){

	}
}

TrapFrame* init_pcb(PCB *p, void *fun){
	TrapFrame *frame = (TrapFrame *)(p->kstack + KSTACK_SIZE) - 1; 
	frame->ds = 0x10;
	frame->es = 0x10;
	frame->fs = 0x10;
	frame->gs = 0x10;
	frame->cs = 8;
	frame->eip = (uint32_t)fun;
	frame->eflags |= IF_MASK;
	p->tf = frame;
	p->list.prev = NULL;
	p->list.next = NULL;
	p->lock_cnt = 0;
	p->outmost_if = IF_MASK;
	list_init(&p->mail);
	create_sem(&p->mail_num, 0);
	return frame;
}