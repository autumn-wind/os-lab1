#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "adt/list.h"

#define KSTACK_SIZE 4096
#define MAXPCB_NUM	10
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
	Sem mail_mutex;
	Sem mail_some[MAXPCB_NUM];
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

extern PCB pcb[MAXPCB_NUM], idle, *current;
extern ListHead ready;
extern pid_t pnum;
 
#endif
