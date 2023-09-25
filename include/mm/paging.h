#ifndef __PAGING_H__
#define __PAGING_H__

#include <stdint.h>


void mmu_init();

void load_ttbr0_el2(uint64_t __page_dir_start);
void load_tcr_el2();
void load_vtcr_el2();
void load_mair_el2();
void clear_el1_tlb();
void enable_mmu();
void disable_mmu();
void prepare_page_tables_and_map_memory(uint64_t __page_dir_start);

struct vaddr_space{
    uint64_t* lv1_table;
};

#endif
