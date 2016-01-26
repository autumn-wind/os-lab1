#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "adt/list.h"

#define INTR assert(read_eflags() & IF_MASK)
#define NOINTR assert(~read_eflags() & IF_MASK)

#define ANY -1

#define KSTACK_SIZE 4096
#define MAXPCB_NUM	20
#define IF_MASK		0x200
#define listhead_to_pcb(ptr) \
	(PCB *)((char *)(ptr) - KSTACK_SIZE - sizeof(void *))
#define listhead_to_mail(ptr) \
	(Msg *)((char *)(ptr) - sizeof(int) * 6 - sizeof(pid_t) * 2)

typedef struct Semaphore {
	int token;
	ListHead block;		/* blocking queue */
}Sem;

typedef struct PCB {
	void *tf;
	uint8_t kstack[KSTACK_SIZE];
	ListHead list;
	uint32_t lock_cnt;
	uint32_t outmost_if;
	pid_t pid;
	ListHead mail;
	Sem mail_num;
} PCB;

typedef struct Message {
	pid_t src, dest;
	union {
		int type;
		int ret;
	};
	union {
		int i[5];
		struct {
			pid_t req_pid;
			int dev_id;
			void *buf;
			off_t offset;
			size_t len;
		};
	};
	ListHead list;
} Msg;

extern void lock();
extern void unlock();
extern void sleep(ListHead *);
extern void wakeup(PCB *);
extern void create_sem(Sem *, int);
extern void P(Sem *);
extern void V(Sem *);
extern void send(pid_t dest, Msg *m);
extern void receive(pid_t src, Msg *m);
extern PCB* create_kthread(void *);
extern PCB* fetch_pcb(pid_t);

extern void copy_from_kernel(PCB* pcb, void* dest, void* src, int len);

extern void copy_to_kernel(PCB* pcb, void* dest, void* src, int len);

extern void strcpy_to_kernel(PCB* pcb, char* dest, char* src);

extern void strcpy_from_kernel(PCB* pcb, char* dest, char* src);

extern PCB pcb[MAXPCB_NUM], idle, *current;
extern ListHead ready;
extern pid_t pnum;
 
#endif
