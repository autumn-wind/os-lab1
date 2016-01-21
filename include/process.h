#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "adt/list.h"

#define KSTACK_SIZE 4096
#define MAXPCB_NUM	10
#define IF_MASK		0x200
#define listhead_to_pcb(ptr) \
	(PCB *)((char *)(ptr) - KSTACK_SIZE - sizeof(void *))

typedef struct PCB {
	void *tf;
	uint8_t kstack[KSTACK_SIZE];
	ListHead list;
	uint32_t lock_cnt;
	uint32_t outmost_if;
} PCB;

typedef struct Semaphore {
	int token;
	ListHead block;		/* blocking queue */
}Sem;

extern PCB *current;

#endif
