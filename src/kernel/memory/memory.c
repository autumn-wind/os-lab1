#include "kernel.h"

void init_mem();
uint8_t mem_maps[PAGE_NR];

void init_mem(){
	int i;
	for(i = 0; i < PAGE_NR; ++ i)
		mem_maps[i] = 0;
}
