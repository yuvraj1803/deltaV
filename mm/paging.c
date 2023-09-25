#include "mm/paging.h"
#include "stdio.h"

extern volatile uint32_t __page_dir_start;

void mmu_init(){

	prepare_page_tables_and_map_memory(&__page_dir_start);
	load_ttbr0_el2(&__page_dir_start);
	load_tcr_el2();
	load_vtcr_el2();
	load_mair_el2();
	clear_el1_tlb();
	//load_sctlr_with_mmu();
	
	log("MMU initialised");
}
