#include "kernel.h"

static void mm(void);

pid_t MM;

void copy_mem(char *dest, char *src, size_t len){
	size_t i;
	for(i = 0; i < len; ++i){
		*(dest + i) = *(src + i);
	}
}

void init_mm(){
	PCB *p = create_kthread(mm);
	MM = p->pid;
	wakeup(p);
}

static void mm(void){
	PDE *pdir, *fdir, *kpdir = get_kpdir();
	PTE *ptable, *ftable;
	uint32_t pdir_idx, ptable_idx, pframe_idx = 0, addr;
	uint32_t pa = 0, va, memsz, i, j, index, user_addr_start;

	Msg m;
	while(1){
		receive(ANY, &m);
		if(m.src == MSG_HARD_INTR){
			assert(0);
		}else if(m.src == PM){
			user_addr_start = m.req_pid * PD_SIZE;
			pdir = pa_to_va(user_addr_start);
			/*ptable = pa_to_va(user_addr_start + PAGE_SIZE);*/
			switch(m.type){
				case CLEAN_ADDR:
					for(pdir_idx = 0; pdir_idx < NR_PDE; pdir_idx++){
						make_invalid_pde(&pdir[pdir_idx]);
					}
					pframe_idx = (user_addr_start >> 12) + 1;
					break;
				case NEW_PAGE:
					assert(pframe_idx);
					/*printk("pframe_idx: %x\n", pframe_idx);*/
					/*printk("pa sent in mm: %x\n", pa);*/
					va = m.offset;
					/*pa = (pframe_idx << 12);*/
					memsz = m.len;
					for(j = 0; j < memsz; j += PD_SIZE){
						index = (va >> 22) & 0x3FF;
						if(pdir[index].present == 0){
							ptable = pa_to_va(pframe_idx << 12);
							make_pde(&pdir[index], (void *)(pframe_idx << 12)); 
							pframe_idx ++;
							for(ptable_idx = 0; ptable_idx < NR_PTE; ptable_idx ++){
								make_invalid_pte(&ptable[ptable_idx]);
							}
						}else{
							ptable = pa_to_va(pdir[index].page_frame << 12);
						}
						pa = (pframe_idx << 12) + (va % PAGE_SIZE);
						for(i = 0; j + i < memsz && i < PD_SIZE; i += PAGE_SIZE, va += PAGE_SIZE){
							make_pte(&ptable[(va >> 12) & 0x3FF], (void *)(pframe_idx << 12));
							pframe_idx ++;
						}
					}
					assert(pa);
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
				case COPY_FATHER_PAGE:
					fdir = pa_to_va(m.offset * PD_SIZE);
					for(i = 0; i < KOFFSET / PD_SIZE; ++i){
						if(fdir[i].present){
							ptable = pa_to_va(pframe_idx << 12);
							make_pde(&pdir[i], (void *)(pframe_idx << 12)); 
							pframe_idx ++;
							for(ptable_idx = 0; ptable_idx < NR_PTE; ptable_idx ++){
								make_invalid_pte(&ptable[ptable_idx]);
							}
							ftable = pa_to_va(fdir[i].page_frame << 12);
							for(j = 0; j < NR_PTE; ++j){
								if(ftable[j].present){
									make_pte(&ptable[j], (void *)(pframe_idx << 12));
									copy_mem((char *)pa_to_va(pframe_idx << 12), (char *)pa_to_va(ftable[j].page_frame << 12), PAGE_SIZE);
									pframe_idx ++;
								}
							}
						}
					}
					break;
				case GET_ARGS_PHY_ADDR:
					addr = m.offset;
					ptable = pa_to_va(pdir[(addr >> 22) & 0x3FF].page_frame << 12);
					m.ret = (ptable[(addr >> 12) & 0x3FF].page_frame << 12) + (addr % PAGE_SIZE);
					break;
				default:
					assert(0);
			}
			pid_t dest = m.src;
			m.src = current->pid;
			send(dest, &m);
		}else{
			assert(0);
		}
	}
	
}
