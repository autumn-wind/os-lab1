#include "kernel.h"

#define INTR assert(read_eflags() & IF_MASK)
#define NOINTR assert(~read_eflags() & IF_MASK)

#define ANY -1
#define pidA 0
#define pidB 1
#define pidC 2
#define pidD 3
#define pidE 4

PCB* create_kthread(void *);
void sleep(ListHead *);
void wakeup(PCB *);
void init_proc();
void lock();
void unlock();
void create_sem(Sem *, int);
void P(Sem *);
void V(Sem *);
void send(pid_t dest, Msg *m);
void receive(pid_t src, Msg *m);
void A();
void B();
void C();
void D();
void E();
void test_setup();

PCB*
create_kthread(void *fun) {
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
	test_setup();
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
	INTR;
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
	INTR;
}

void V(Sem *s){
	INTR;
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
	INTR;
}

void send(pid_t dest, Msg *m){
	m->dest = dest;
	lock();
	list_add_before(&pcb[dest].mail, &m->list);
	unlock();
	V(&pcb[dest].mail_num);INTR;
}

void receive(pid_t src, Msg *m){
	ListHead *pmail;
	Msg *msg;
	uint8_t flag = 0, count = 0;
	while(!flag){
		P(&current->mail_num);INTR;
		if(src >= 0){
			lock();NOINTR;
			for(pmail = current->mail.next; pmail != &current->mail; pmail = pmail->next){
				msg = listhead_to_mail(pmail);
				if(msg->src == src){
					list_del(pmail);
					m->src = msg->src;
					flag = 1;
					break;
				}
			}
			unlock();INTR;
		}else if(src == ANY){
			lock();NOINTR;
			pmail = current->mail.next;
			list_del(pmail);
			msg = listhead_to_mail(pmail);
			m->src = msg->src;
			flag = 1;
			unlock();INTR;
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

void A () { 
	Msg m1, m2;
	m1.src = current->pid;
	int x = 0;
	while(1) {
		if(x % 10000000 == 0) {
			printk("a"); 
			send(pidE, &m1);
			receive(pidE, &m2);
		}
		x ++;
	}
}
void B () { 
	Msg m1, m2;
	m1.src = current->pid;
	int x = 0;
	receive(pidE, &m2);
	while(1) {
		if(x % 10000000 == 0) {
			printk("b"); 
			send(pidE, &m1);
			receive(pidE, &m2);
		}
		x ++;
	}
}
void C () { 
	Msg m1, m2;
	m1.src = current->pid;
	int x = 0;
	receive(pidE, &m2);
	while(1) {
		if(x % 10000000 == 0) {
			printk("c"); 
			send(pidE, &m1);
			receive(pidE, &m2);
		}
		x ++;
	}
}
void D () { 
	Msg m1, m2;
	m1.src = current->pid;
	receive(pidE, &m2);
	int x = 0;
	while(1) {
		if(x % 10000000 == 0) {
			printk("d"); 
			send(pidE, &m1);
			receive(pidE, &m2);
		}
		x ++;
	}
}

void E () {
	Msg m1, m2;
	m2.src = current->pid;
	char c;
	while(1) {
		receive(ANY, &m1);
		if(m1.src == pidA) {c = '|'; m2.dest = pidB; }
		else if(m1.src == pidB) {c = '/'; m2.dest = pidC;}
		else if(m1.src == pidC) {c = '-'; m2.dest = pidD;}
		else if(m1.src == pidD) {c = '\\';m2.dest = pidA;}
		else assert(0);

		printk("\033[s\033[1000;1000H%c\033[u", c);
		send(m2.dest, &m2);
	}
}

void test_setup(){
	wakeup(create_kthread(A));
	wakeup(create_kthread(B));
	wakeup(create_kthread(C));
	wakeup(create_kthread(D));
	wakeup(create_kthread(E));
}

