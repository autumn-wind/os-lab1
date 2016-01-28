#include "kernel.h"

void lock();
void unlock();
void sleep(ListHead *);
void wakeup(PCB *);
void create_sem(Sem *, int);
void P(Sem *);
void V(Sem *);
void send(pid_t dest, Msg *m);
void receive(pid_t src, Msg *m);
void init_proc();
void read_mbr();

PCB*
create_kthread(void *fun) {
	if(pnum >= MAXPCB_NUM){
		panic("no pcb for more thread");
	}
	PCB *p = &pcb[pnum];
	TrapFrame *frame = (TrapFrame *)(p->kstack + KSTACK_SIZE) - 1; 
	frame->ds = 0x10;
	frame->es = 0x10;
	frame->fs = 0x10;
	frame->gs = 0x10;
	frame->cs = 8;
	frame->eip = (uint32_t)fun;
	frame->eflags |= IF_MASK;
	p->tf = frame;
	(p->list).prev = NULL;
	(p->list).next = NULL;
	p->lock_cnt = 0;
	p->outmost_if = IF_MASK;
	p->pid = pnum++;
	list_init(&p->mail);
	create_sem(&p->mail_num, 0);
	return p;
}

void sleep(ListHead *b){
	lock();
	NOINTR;
	list_del(&current->list);
	list_add_before(b, &current->list);
	unlock();
	asm volatile("int $0x80");
}

void wakeup(PCB *p){
	lock();
	NOINTR;
	list_del(&p->list);
	list_add_before(&idle.list, &p->list);
	unlock();
}

void
init_proc() {
	list_init(&ready);
	list_add_before(&ready, &idle.list);
	wakeup(create_kthread(read_mbr));
}

void lock(){
	if(current->lock_cnt == 0)
		current->outmost_if = (read_eflags() & IF_MASK);	
	cli();
	current->lock_cnt += 1;
}

void unlock(){
	current->lock_cnt -= 1;
	if(current->lock_cnt == 0)
		if(current->outmost_if)
			sti();
}

void create_sem(Sem *sem, int t){
	sem->token = t;
	list_init(&(sem->block));
}

void P(Sem *s){
	lock();
	NOINTR;
	if(s->token > 0)
		s->token -= 1;
	else
	{
		sleep(&(s->block));
		NOINTR;
	}
	unlock();
}

void V(Sem *s){
	lock();
	NOINTR;
	if(list_empty(&(s->block)))
		s->token += 1;
	else
	{
		wakeup(listhead_to_pcb((s->block).next));
		NOINTR;
	}
	unlock();
}

void send(pid_t dest, Msg *m){
	m->dest = dest;
	lock();
	list_add_before(&pcb[dest].mail, &m->list);
	unlock();
	V(&pcb[dest].mail_num);
}

void copy_msg(Msg *d, Msg *s){
	d->src = s->src;
	d->dest = s->dest;
	d->type = s->type;
	d->req_pid = s->req_pid;
	d->dev_id = s->dev_id;
	d->buf = s->buf;
	d->offset = s->offset;
	d->len = s->len;
	d->list = s->list;
}

void receive(pid_t src, Msg *m){
	ListHead *pmail;
	Msg *msg;
	uint8_t flag = 0, count = 0;
	while(!flag){
		P(&current->mail_num);
		if(src != ANY){
			lock();NOINTR;
			for(pmail = current->mail.next; pmail != &current->mail; pmail = pmail->next){
				msg = listhead_to_mail(pmail);
				if(msg->src == src){
					list_del(pmail);
					copy_msg(m, msg);
					flag = 1;
					break;
				}
			}
			unlock();
		}else if(src == ANY){
			lock();NOINTR;
			pmail = current->mail.next;
			list_del(pmail);
			msg = listhead_to_mail(pmail);
			copy_msg(m, msg);
			flag = 1;
			unlock();
		}else{
			/*should never come here*/
			assert(0);
		}
		if(!flag){
			count += 1;
		}
	}
	lock();
	current->mail_num.token += count;
	unlock();
}

void read_mbr(){
	uint8_t buf[512];
	int n = dev_read("hda", current->pid, buf, 0, 512);
	if(n)
		printk("read %d bytes in MBR:\n", n);
	else
		panic("read ide failed!\n");
	int i;
	for(i = 0; i < 512; ++i){
		uint8_t c = buf[i];
		serial_printc(hexBoard[(c & 0xf0) >> 4]);
		serial_printc(hexBoard[(c & 0x0f)]);
		serial_printc(' ');
	}
	while(1){

	}
}
