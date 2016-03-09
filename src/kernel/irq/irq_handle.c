#include "kernel.h"

#define NR_IRQ_HANDLE 32

/* In Nanos, there are no more than 16(actually, 3) hardware interrupts. */
#define NR_HARD_INTR 16

/* Structures below is a linked list of function pointers indicating the
   work to be done for each interrupt. Routines will be called one by
   another when interrupt comes.
   For example, the timer interrupts are used for alarm clocks, so
   device driver of timer will use add_irq_handle to attach a simple
   alarm checker. Also, the IDE driver needs to write back dirty cache
   slots periodically. So the IDE driver also calls add_irq_handle
   to register a handler. */
#define SYS_puts 0
#define SYS_fork 1
#define SYS_exec 2
#define SYS_exit 3
#define SYS_gets 4
#define SYS_waitpid 5

struct IRQ_t {
	void (*routine)(void);
	struct IRQ_t *next;
};

void do_syscall(TrapFrame *tf);

static struct IRQ_t handle_pool[NR_IRQ_HANDLE];
static struct IRQ_t *handles[NR_HARD_INTR];
static int handle_count = 0;
static char buf[PAGE_SIZE];

void do_syscall(TrapFrame *tf) {
	int id = tf->eax; // system call id
	Msg m;
	int n, i;
	pid_t wait_id;
	char *c;
 
	switch (id) {
		case SYS_puts:
			printk("%s\ncurrent process: %d\n\n", tf->ebx, current->pid);
			break;
		case SYS_fork:
			m.src = current->pid;
			m.type = FORK_PROCESS;
			m.buf = tf;
			send(PM, &m);
			receive(PM, &m);
			tf->eax = m.ret;
			/*printk("child pid: %d\n", m.ret);*/
			break;
		case SYS_exec:
			m.src = current->pid;
			m.type = EXEC_PROCESS;
			m.req_pid = tf->ebx;
			m.offset = tf->ecx;
			send(PM, &m);
			receive(PM, &m);
			break;
		case SYS_exit:
			m.src = current->pid;
			m.type = EXIT_PROCESS;
			send(PM, &m);
			receive(PM, &m);
			break;
		case SYS_gets:
			n = dev_read("tty1", current->pid, buf, 0, PAGE_SIZE);
			c = (char *)( tf->ebx );
			for(i = 0; i < n; ++ i){
				c[i] = buf[i];
			}
			c[n] = '\0';
			tf->eax = n;
			break;
		case SYS_waitpid:
			wait_id = tf->ebx;
			lock();
			if(pid_pool[wait_id] == 1){
				m.src = current->pid;
				m.type = WAIT_PID;
				m.req_pid = wait_id;
				send(PM, &m);
				receive(PM, &m);
				tf->eax = 1;
			}else{
				tf->eax = 0;
			}
			unlock();
			break;
	}
}

void
add_irq_handle(int irq, void (*func)(void) ) {
	struct IRQ_t *ptr;
	assert(irq < NR_HARD_INTR);
	if (handle_count > NR_IRQ_HANDLE) {
		panic("Too many irq registrations!");
	}
	ptr = &handle_pool[handle_count ++]; /* get a free handler */
	ptr->routine = func;
	ptr->next = handles[irq]; /* insert into the linked list */
	handles[irq] = ptr;
}
void schedule();

void irq_handle(TrapFrame *tf) {
	/*printk("current->tf: %x\tTrapFrame tf: %x\terror_code: %x\n", current->tf, tf, tf->error_code);printk("pid: %d\tirq: %d\teflags: %x\tcs: %x\teip: %x\tebx:%x\n\n", current->pid, tf->irq, tf->eflags, tf->cs, tf->eip, tf->ebx);*/
	int irq = tf->irq;

	if (irq < 0) {
		panic("Unhandled exception!");
	}
	
	if(irq == 0x80 && tf->eax == -2)
		printk("test3 past!\ncurrent process: %d\n\n", current->pid);

	if (irq < 1000 && irq != 0x80) {
		extern uint8_t logo[];
		panic("Unexpected exception #%d\n\33[1;31mHint: The machine is always right! For more details about exception #%d, see\n%s\n\33[0m", irq, irq, logo);
	} else if (irq >= 1000) {
		/* The following code is to handle external interrupts.
		 * You will use this code in Lab2.  */
		int irq_id = irq - 1000;
		assert(irq_id < NR_HARD_INTR);
		struct IRQ_t *f = handles[irq_id];

		while (f != NULL) { /* call handlers one by one */
			f->routine(); 
			f = f->next;
		}
	} else if(irq == 0x80) {
		do_syscall(tf);
	}
	NOINTR;

	current->tf = tf;
	schedule();
}

