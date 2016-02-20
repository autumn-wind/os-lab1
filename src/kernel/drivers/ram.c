#include "kernel.h"
#include "x86/x86.h"
#include "hal.h"
#include "time.h"
#include "string.h"

static uint8_t file[NR_MAX_FILE][NR_FILE_SIZE] = {
	{0x12, 0x34, 0x56, 0x78},	// the first file '0'
	{"Hello Beautiful World!\n"},		// the second file '1'
	{0x7f, 0x45, 0x4c, 0x46}	// the third file '2'
};
static uint8_t *disk = (void*)file;

pid_t RAM;

static void ram_driver_thread(void);
static void read_ram_request(Msg *m);

void init_ram(void){
	PCB *p = create_kthread(ram_driver_thread);
	RAM = p->pid;
	wakeup(p);
	hal_register("ram", RAM, 0);
}

static void
ram_driver_thread(void){
	Msg m;
	while(1){
		receive(ANY, &m);
		if(m.src == MSG_HARD_INTR){
			assert(0);
		}else{
			switch(m.type){
				case DEV_READ:
					read_ram_request(&m);
					break;
				default:
					assert(0);
			}
		}
	}
}

static void read_ram_request(Msg *m){
	uint8_t *buf = m->buf;
	off_t offset = m->offset;
	size_t len = m->len;
	int i;
	for(i = 0; i < len; ++i)
		buf[i] = disk[offset + i];
	m->ret = m->len;
	pid_t dest = m->src;
	m->src = current->pid;
	send(dest, m);
}
