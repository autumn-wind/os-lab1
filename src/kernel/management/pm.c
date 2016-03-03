#include "kernel.h"

char buf_for_args[PAGE_SIZE];

static void pm(void);
TrapFrame* init_pcb(PCB *p, void *fun, char *args);
void create_user_process(int file, PCB *p, char *args);
void copy_process(PCB *father, PCB *child);
void clean_process_addr(Msg *m, pid_t src, int req_pid);
void copy_father_page(Msg *m, pid_t src ,pid_t father_pid, pid_t req_pid);
void share_kernel_page(Msg *m, pid_t src, pid_t req_pid);

pid_t PM;

void init_pm(){
	PCB *p = create_kthread(pm);
	PM = p->pid;
	wakeup(p);
}

void clean_process_addr(Msg *m, pid_t src, int req_pid){
	m->src = src;
	m->type = CLEAN_ADDR;
	m->req_pid = req_pid;
	send(MM, m);
	receive(MM, m);
}

void create_user_process(int file, PCB *p, char *args){
	uint32_t va, pa;
	unsigned char *i, *vdest;
	char buf[512];
	Msg m;
	do_read(file, (uint8_t *)buf, 0, 512);
	clean_process_addr(&m, current->pid, p->pid);

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
		m.req_pid = p->pid;
		m.offset = va;
		m.len = ph->memsz;
		send(MM, &m);
		receive(MM, &m);
		pa = m.ret;
		/*printk("pa receiverd in pm: %x\n", pa);*/
		vdest = pa_to_va(pa);
		do_read(file, vdest, ph->off, ph->filesz);
		for(i = vdest + ph->filesz; i < vdest + ph->memsz; *i++ = 0);
	}
	init_pcb(p, (void *)(elf->entry), args);	
	p->cr3.val = 0;
	p->cr3.page_directory_base = (p->pid * PD_SIZE) >> 12;
	/*printk("user cr3: %x\n", p->cr3.page_directory_base);*/
	share_kernel_page(&m, current->pid, p->pid);
	wakeup(p);

}

void copy_process(PCB *father, PCB *child){
	child->lock_cnt = father->lock_cnt;
	child->outmost_if = father->outmost_if;
	child->list.prev = child->list.next = NULL;
	list_init(&child->mail);
	create_sem(&child->mail_num, 0);
}

void copy_father_page(Msg *m, pid_t src ,pid_t father_pid, pid_t req_pid){
	m->src = src;
	m->type = COPY_FATHER_PAGE;
	m->req_pid = req_pid;
	m->offset = father_pid;
	send(MM, m);
	receive(MM, m);
}

void share_kernel_page(Msg *m, pid_t src, pid_t req_pid){
	m->src = src;
	m->type = SHARE_KERNEL_PAGE;
	m->req_pid = req_pid;
	m->offset = KOFFSET;
	m->len = KMEM;
	send(MM, m);
	receive(MM, m);
}

uint32_t get_args_phy_addr(Msg *m, pid_t old, uint32_t addr){
	m->src = current->pid;
	m->type = GET_ARGS_PHY_ADDR;
	m->req_pid = old;
	m->offset = addr;
	send(MM, m);
	receive(MM, m);
	return m->ret;
}

void reclaim_resources(PCB *p){
	lock();
	ListHead *ptr, *temp;
	for(ptr = p->mail.next; ptr != &p->mail; ptr = temp){
		temp = ptr->next;
		list_del(ptr);
		list_add_before(&msg_pool, ptr);
	}
	unlock();
}

static void pm(void){
	create_user_process(0, get_pcb(), 0);
	/*create_user_process(3, get_pcb(), 0);*/
	Msg m;
	while(1){
		receive(ANY, &m);
		if(m.type == FORK_PROCESS){
			Msg m2;
			pid_t father_pid = m.src, child_pid;
			PCB *child = get_pcb(), *father = fetch_pcb(father_pid);
			child_pid = child->pid;
			TrapFrame *ftf = m.buf;
			copy_process(father, child);
			clean_process_addr(&m2, current->pid, child_pid);
			copy_father_page(&m2, current->pid, father_pid, child_pid);
			share_kernel_page(&m2, current->pid, child_pid);
			child->cr3.val = 0;
			child->cr3.page_directory_base = (child_pid * PD_SIZE) >> 12;
			off_t offset = (uint32_t)((PCB *)0 + 1) * (child_pid - father_pid);
			TrapFrame *ptf = (TrapFrame *)((char *)ftf + offset);
			uint32_t stack_len = (uint32_t)(father->kstack) + KSTACK_SIZE - (uint32_t)ftf;
			copy_mem((char *)ptf, (char *)ftf, stack_len);
			uint32_t *ebp = &ftf->ebp;
			while(*ebp != 0){
				*(uint32_t *)((uint32_t)ebp + offset) = *ebp + offset;
				ebp = (uint32_t *)(*ebp);
			}
			/*assert(0);*/
			*(uint32_t *)((uint32_t)ebp + offset) = 0;
			child->tf = ptf;
			ptf->eax = 0;
			wakeup(child);
			int dest = m.src;
			m.src = current->pid;
			m.ret = child_pid;
			send(dest, &m);
		}else if(m.type == EXEC_PROCESS){
			Msg m2;
			pid_t old = m.src;
			int file = m.req_pid;
			uint32_t args_addr = m.offset;
			uint32_t args_phy_addr = get_args_phy_addr(&m2, old, args_addr);
			char *kargs = (char *)pa_to_va(args_phy_addr);
			int i;
			for(i = 0; *(kargs + i) != '\0'; ++i)
				buf_for_args[i] = *(kargs + i);
			buf_for_args[i] = *(kargs + i);
			PCB *p = fetch_pcb(old);
			reclaim_resources(p);
			create_user_process(file, p, buf_for_args);
			
		}else if(m.type == EXIT_PROCESS){
			lock();
			pid_t pid = m.src;
			PCB *p = fetch_pcb(pid);
			reclaim_resources(p);
			list_del(&p->list);
			list_add_before(&pcb_pool, &p->list);
			p->pid = -1;
			pid_pool[pid] = 0;

			unlock();
		}else{
			assert(0);
		}
	}
}

TrapFrame* init_pcb(PCB *p, void *fun, char *args){
	TrapFrame *frame = (TrapFrame *)(p->kstack + KSTACK_SIZE) - 1; 
	frame->ebp = 0;
	frame->ds = 0x10;
	frame->es = 0x10;
	frame->fs = 0x10;
	frame->gs = 0x10;
	frame->cs = 8;
	frame->eip = (uint32_t)fun;
	frame->eflags |= IF_MASK;
	frame->args = (uint32_t)args;
	p->tf = frame;
	p->list.prev = NULL;
	p->list.next = NULL;
	p->lock_cnt = 0;
	p->outmost_if = IF_MASK;
	list_init(&p->mail);
	create_sem(&p->mail_num, 0);
	return frame;
}
