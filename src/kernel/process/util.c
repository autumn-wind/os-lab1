#include "kernel.h"

void lock();
void unlock();
void sleep(ListHead *);
void wakeup(PCB *);
void create_sem(Sem *, int);
void P(Sem *);
void V(Sem *);
void copy_msg(Msg *d, Msg *s);
void send(pid_t dest, Msg *m);
void receive(pid_t src, Msg *m);
void init_proc();
void read_mbr();
void read_ram();
void read_file();

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
	p->list.prev = NULL;
	p->list.next = NULL;
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
	wakeup(create_kthread(read_file));
	list_init(&msg_pool);
	int i = 0;
	for(i = 0; i < MAXMSG_NUM; ++i)
		list_add_before(&msg_pool, &msgs[i].list);
}

void lock(){
	assert(current->lock_cnt >= 0);
	if(current->lock_cnt == 0)
		current->outmost_if = (read_eflags() & IF_MASK);	
	cli();
	current->lock_cnt += 1;
	assert(current->lock_cnt >= 0);
}

void unlock(){
	assert(current->lock_cnt >= 0);
	current->lock_cnt -= 1;
	if(current->lock_cnt == 0)
		if(current->outmost_if)
			sti();
	assert(current->lock_cnt >= 0);
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
		wakeup(list_entry((s->block).next, PCB, list));
		NOINTR;
	}
	unlock();
}

/*#define DEBUG*/

void send(pid_t dest, Msg *m){
	m->dest = dest;
	lock();
	if(list_empty(&msg_pool))
		panic("message pool runs out!\n");
	ListHead *pm = msg_pool.next;
	list_del(pm);
	Msg *t = list_entry(pm, Msg, list);
	copy_msg(t, m);
#ifdef DEBUG
	printk("current pid: %d\n", current->pid);
	printk("send a message:\n");
	printk("message addr:%x\n", m);
	printk("message src: %d\n", m->src);
	printk("message dest: %d\n", m->dest);
	printk("message type: %d\n", m->type);
	printk("\n");
#endif
	/*printk("sending...\n");*/
	/*printk("sender: %d\treceiver:%d\n", current->pid, dest);*/
	/*printk("%x\n\n", m);*/
	list_add_before(&pcb[dest].mail, &t->list);
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
	/*d->list = s->list;*/
}

void receive(pid_t src, Msg *m){
	ListHead *pmail;
	Msg *msg;
	int flag = 0, count = 0;
	/*printk("receiving...\n\n");*/
	while(!flag){
		P(&current->mail_num);
		if(src != ANY){
			/*printk("1	%x\n", &current->mail);*/
			lock();NOINTR;
			if(list_empty(&current->mail))
				panic("mail box is empty!!\n");
#ifdef DEBUG
			int mail_cnt = 0;
			list_foreach(pmail, &current->mail)
				mail_cnt++;
			printk("mail_cnt: %d\t mail_num: %d\t count: %d\n", mail_cnt, current->mail_num.token, count);
#endif
			list_foreach(pmail, &current->mail){
				/*printk("2\n");*/
				/*printk("receiver: %d\tsender: %d\n", current->pid, src);*/
				/*printk("%x	%x\n", pmail, &current->mail);*/
				msg = list_entry(pmail, Msg, list);
				if(msg->src == src){
					list_del(pmail);
					copy_msg(m, msg);
#ifdef DEBUG
					printk("current pid: %d\n", current->pid);
					printk("receive certain message:\n");
					printk("message addr:%x\n", msg);
					printk("message src: %d\n", m->src);
					printk("message dest: %d\n", m->dest);
					printk("message type: %d\n", m->type);
					printk("\n");
#endif
					flag = 1;
					break;
				}
			}
			unlock();
		}else if(src == ANY){
			lock();NOINTR;
			if(list_empty(&current->mail))
				panic("mail box is empty!!\n");
#ifdef DEBUG
			int mail_cnt = 0;
			list_foreach(pmail, &current->mail)
				mail_cnt++;
			printk("mail_cnt: %d\t mail_num: %d\t count: %d\n", mail_cnt, current->mail_num.token, count);
#endif
			pmail = current->mail.next;
			list_del(pmail);
			msg = list_entry(pmail, Msg, list);
			copy_msg(m, msg);
#ifdef DEBUG
			printk("current pid: %d\n", current->pid);
			printk("receive any message:\n");
			printk("message addr:%x\n", msg);
			printk("message src: %d\n", m->src);
			printk("message dest: %d\n", m->dest);
			printk("message type: %d\n", m->type);
			printk("\n");
#endif
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
	/*printk("3\n\n");*/
	lock();
	list_add_before(&msg_pool, pmail);
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
	serial_printc('\n');
	while(1){

	}
}

void read_ram(){
	int len = 1024, offset = 0;
	uint8_t buf[len];
	int n = dev_read("ram", current->pid, buf, offset, len);
	if(n)
		printk("read %d bytes in ramdisk:\n", n);
	else
		panic("read ramdisk failed!\n");
	int i;
	for(i = 0; i < len; ++i){
		printk("%c", buf[i]);
	}
	printk("\n");
	while(1){

	}
}

void read_file(){
	int file = 1, len = 1024, offset = 0;
	uint8_t buf[len];
	int n = do_read(file, buf, offset, len);
	if(n)
		printk("read %d bytes from file %d:\n", n, file);
	else
		panic("read file %d failed!\n", file);
	int i;
	for(i = 0; i < len; ++i){
		printk("%c", buf[i]);
	}
	printk("\n");
	while(1){

	}
}
