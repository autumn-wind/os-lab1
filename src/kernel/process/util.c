#include "kernel.h"

#define INTR assert(read_eflags() & IF_MASK)
#define NOINTR assert(~read_eflags() & IF_MASK)

PCB* create_kthread(void *);
void sleep(ListHead *);
void wakeup(PCB *);
void init_proc();
void test_producer();
void test_consumer();
void test_setup();
void lock();
void unlock();
void create_sem(Sem *, int);
void P(Sem *);
void V(Sem *);

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
	frame->eflags |= IF_MASK;
	p->tf = frame;
	(p->list).prev = NULL;
	(p->list).next = NULL;
	p->lock_cnt = 0;
	p->outmost_if = IF_MASK;
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

#define NBUF 5
#define NR_PROD 3
#define NR_CONS 4

int buf[NBUF], f = 0, r = 0, g = 1;
int last = 0;
Sem empty, full, mutex;

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

void
test_producer(void) {
	while (1) {
		P(&empty);INTR;
		P(&mutex);INTR;
		if(g % 10000 == 0) {
			printk(".");	// tell us threads are really working
		}
		buf[f ++] = g ++;
		f %= NBUF;
		V(&mutex);INTR;
		V(&full);INTR;
	}
}

void
test_consumer(void) {
	int get;
	while (1) {
		P(&full);INTR;
		P(&mutex);INTR;
		get = buf[r ++];
		assert(last == get - 1);	// the products should be strictly increasing
		last = get;
		r %= NBUF;
		V(&mutex);INTR;
		V(&empty);INTR;
	}
}

void
test_setup(void) {
	create_sem(&full, 0);
	create_sem(&empty, NBUF);
	create_sem(&mutex, 1);
	int i;
	for(i = 0; i < NR_PROD; i ++) {
		wakeup(create_kthread(test_producer));
	}
	for(i = 0; i < NR_CONS; i ++) {
		wakeup(create_kthread(test_consumer));
	}
}

