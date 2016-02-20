#include "kernel.h"

pid_t FM;

size_t do_read(int file_name, uint8_t *buf, off_t offset, size_t len);
static void fm(void);

void init_fm(){
	PCB *p = create_kthread(fm);
	FM = p->pid;
	wakeup(p);
}

static void fm(void){
	Msg m;
	while(1){
		receive(ANY, &m);
		if(m.src == MSG_HARD_INTR){
			assert(0);
		}else{
			size_t n = dev_read("ram", m.req_pid, m.buf, m.type * NR_FILE_SIZE + m.offset, m.len);
			m.ret = n;
			pid_t dest = m.src;
			m.src = current->pid;
			send(dest, &m);
		}
	}
}

size_t do_read(int file_name, uint8_t *buf, off_t offset, size_t len){
	Msg m;
	m.src = m.req_pid = current->pid;
	m.type = file_name;
	m.buf = buf;
	m.offset = offset;
	m.len = len;
	send(FM, &m);
	receive(FM, &m);
	return m.ret;
}
