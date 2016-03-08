#include "kernel.h"

static void mm(void);

extern uint8_t mem_maps[PAGE_NR];

pid_t MM;

uint32_t get_free_page(){
	int i;
	for(i = 0; i < PAGE_NR; ++ i){
		if(mem_maps[i] == 0){
			mem_maps[i] = 1;
			return KMEM + i * PAGE_SIZE;
		}
	}
	panic("No free page is found!\n");
	return 0;
}

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
	uint32_t pdir_idx, ptable_idx, addr, stack_page, new_page;
	uint32_t va, memsz, filesz, phoff, i, j, k, index;
	int file;
	uint8_t *c;

	Msg m;
	while(1){
		receive(ANY, &m);
		if(m.src == MSG_HARD_INTR){
			assert(0);
		}else if(m.src == PM){
			/*user_addr_start = m.req_pid * PD_SIZE;*/
			/*pdir = pa_to_va(user_addr_start);*/
			/*ptable = pa_to_va(user_addr_start + PAGE_SIZE);*/
			switch(m.type){
				case GET_PAGE_DIR:
					pdir = pa_to_va( get_free_page() );
					/*printk("page dir: %x\n", va_to_pa(pdir));*/
					for(pdir_idx = 0; pdir_idx < NR_PDE; ++ pdir_idx){
						make_invalid_pde(&pdir[pdir_idx]);
					}
					m.ret = (uint32_t)va_to_pa(pdir);
					break;
				case CLEAN_ADDR:
					pdir = pa_to_va(m.buf);
					for(pdir_idx = 0; pdir_idx < KOFFSET / PD_SIZE; pdir_idx++){
						if(pdir[pdir_idx].present == 1){
							ptable = pa_to_va(pdir[pdir_idx].page_frame << 12);
							for(ptable_idx = 0; ptable_idx < NR_PTE; ++ ptable_idx){
								if(ptable[ptable_idx].present == 1){
									mem_maps[( ( ptable[ptable_idx].page_frame << 12 ) - KMEM ) / PAGE_SIZE] = 0;
								}
							}
							mem_maps[((pdir[pdir_idx].page_frame << 12) - KMEM) / PAGE_SIZE] = 0;
						}
					}
					mem_maps[( (uint32_t)va_to_pa(pdir) - KMEM ) / PAGE_SIZE] = 0;
					/*pframe_idx = (user_addr_start >> 12) + 1;*/
					break;
				case GET_STACK_PAGE:
					pdir = pa_to_va(m.buf);
					index = (USER_STACK >> 22) & 0x3FF;
					if(pdir[index].present){
						ptable = pa_to_va( pdir[index].page_frame << 12 );
					}else{
						ptable = pa_to_va(get_free_page());
						/*printk("new page table for stack: %x\n", va_to_pa(ptable));*/
						for(ptable_idx = 0; ptable_idx < NR_PTE; ptable_idx ++){
							make_invalid_pte(&ptable[ptable_idx]);
						}
						make_pde(&pdir[( USER_STACK >> 22 ) & 0x3FF], va_to_pa(ptable));
					}
					stack_page = get_free_page();
					/*printk("stack_page: %x\n", stack_page);*/
					make_pte(&ptable[ ( USER_STACK >> 12 ) & 0x3FF], (void *)stack_page);
					m.ret = stack_page;
					break;
				case NEW_PAGE:
					pdir = pa_to_va(m.buf);
					/*printk("pframe_idx: %x\n", pframe_idx);*/
					/*printk("pa sent in mm: %x\n", pa);*/
					va = m.offset;
					file = m.req_pid;
					/*pa = (pframe_idx << 12);*/
					memsz = m.len;
					filesz = m.filesz;
					/*printk("filesz: %x\n", filesz);*/
					phoff = m.phoff;
					for(j = 0; j < memsz; ){
						index = (va >> 22) & 0x3FF;
						if(pdir[index].present == 0){
							ptable = pa_to_va(get_free_page());
							/*printk("new page table: %x\n", va_to_pa(ptable));*/
							for(ptable_idx = 0; ptable_idx < NR_PTE; ptable_idx ++){
								make_invalid_pte(&ptable[ptable_idx]);
							}
							make_pde(&pdir[index], va_to_pa(ptable)); 
						}else{
							ptable = pa_to_va(pdir[index].page_frame << 12);
						}
						off_t page_offset;
						for(i = 0; j + i < memsz && i <= PD_SIZE - PAGE_SIZE; ){
							page_offset = va % PAGE_SIZE;
							size_t can_fill = PAGE_SIZE - page_offset;
							new_page = get_free_page();
							/*printk("new page: %x\n", new_page);*/
							make_pte(&ptable[(va >> 12) & 0x3FF], (void *)new_page);
							c = pa_to_va(new_page);
							if(j + i >= filesz){
								for(k = page_offset; k < PAGE_SIZE; ++ k){
									c[k] = 0;
								}
								/*printk("fill zero page\n");*/
							}else{
								if(j + i + can_fill < filesz){
									do_read(file, c, phoff, can_fill);
									phoff += can_fill;
									/*printk("fill full page\n");*/
								}else{
									size_t len = filesz - (j + i);
									do_read(file, c + page_offset, phoff, len);
									for(k = page_offset + len; k < PAGE_SIZE; ++ k){
										c[k] = 0;
									}
									/*printk("fill half-full page\n");*/
								}
							}
							va += can_fill;
							i += can_fill;
						}
						j += i;
					}
					break;
				case SHARE_KERNEL_PAGE:
					pdir = pa_to_va(m.buf);
					va = KOFFSET;
					memsz = KMEM;
					for(i = 0; i < memsz; i += PD_SIZE, va += PD_SIZE){
						index = (va >> 22) & 0x3FF;
						make_pde(&pdir[index], (void *)(kpdir[index].page_frame << 12));
					}
					break;
				case COPY_FATHER_PAGE:
					pdir = pa_to_va(m.buf);
					fdir = pa_to_va(m.offset);
					for(i = 0; i < KOFFSET / PD_SIZE; ++i){
						if(fdir[i].present){
							if(pdir[i].present){
								ptable = pa_to_va(pdir[i].page_frame << 12);
							}else{
								ptable = pa_to_va(get_free_page());
								for(ptable_idx = 0; ptable_idx < NR_PTE; ptable_idx ++){
									make_invalid_pte(&ptable[ptable_idx]);
								}
								/*printk("new child page table: %x\n", va_to_pa(ptable));*/
								make_pde(&pdir[i], va_to_pa(ptable)); 
							}
							ftable = pa_to_va(fdir[i].page_frame << 12);
							for(j = 0; j < NR_PTE; ++j){
								if(ftable[j].present){
									uint32_t phy_page;
									if(ptable[j].present){
										phy_page = ptable[j].page_frame << 12;
									}else{
										phy_page = get_free_page();
										/*printk("new child page: %x\n", phy_page);*/
										make_pte(&ptable[j], (void *)phy_page);
									}
									copy_mem((char *)pa_to_va(phy_page), (char *)pa_to_va(ftable[j].page_frame << 12), PAGE_SIZE);
								}
							}
						}
					}
					break;
				case GET_ARGS_PHY_ADDR:
					pdir = pa_to_va(m.buf);
					addr = m.offset;
					/*printk("old process args virtual addr: %x\n", addr);*/
					ptable = pa_to_va(pdir[(addr >> 22) & 0x3FF].page_frame << 12);
					m.ret = (ptable[(addr >> 12) & 0x3FF].page_frame << 12) + (addr % PAGE_SIZE);
					/*printk("old process args physical addr: %x\n", m.ret);*/
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
