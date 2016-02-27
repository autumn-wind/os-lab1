#include "kernel.h"

#define SEGMENT_MEM 4 * 1024 * 1024
#define USER_ADDR 10 * SEGMENT_MEM


static void mm(void);

pid_t MM;

void init_mm(){
	PCB *p = create_kthread(mm);
	MM = p->pid;
	wakeup(p);
}

static void mm(void){
	PDE *pdir = pa_to_va(USER_ADDR), *kpdir = get_kpdir();
	PTE *ptable = pa_to_va(USER_ADDR + PAGE_SIZE);
	uint32_t pdir_idx, pframe_idx;

	for(pdir_idx = 0; pdir_idx < NR_PDE; pdir_idx++){
		make_invalid_pde(&pdir[pdir_idx]);
	}

	pframe_idx = (USER_ADDR >> 12) + 2;
	/*printk("pframe_idx: %x\n", pframe_idx);*/
	uint32_t pa, va, memsz, i, index;

	Msg m;
	while(1){
		receive(ANY, &m);
		if(m.src == MSG_HARD_INTR){
			assert(0);
		}else{
			switch(m.type){
				case NEW_PAGE:
					/*printk("pframe_idx: %x\n", pframe_idx);*/
					/*printk("pa sent in mm: %x\n", pa);*/
					va = m.offset;
					/*pa = (pframe_idx << 12);*/
					pa = (pframe_idx << 12) + (va % PAGE_SIZE);
					memsz = m.len;
					make_pde(&pdir[(va >> 22) & 0x3FF], va_to_pa(ptable)); 
					for(i = 0; i < memsz; i += PAGE_SIZE, va += PAGE_SIZE){
						make_pte(&ptable[(va >> 12) & 0x3FF], (void *)(pframe_idx << 12));
						pframe_idx ++;
					}
					m.ret = pa;
					break;
				case SHARE_KERNEL_PAGE:
					va = m.offset;
					memsz = m.len;
					for(i = 0; i < memsz; i += PD_SIZE, va += PD_SIZE){
						index = (va >> 22) & 0x3FF;
						make_pde(&pdir[index], (void *)(kpdir[index].page_frame << 12));
					}
					break;
				default:
					assert(0);
			}
			pid_t dest = m.src;
			m.src = current->pid;
			send(dest, &m);
		}
	}
	
}
