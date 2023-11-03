/*
 *
 * Copyright (c) 2023 Yuvraj Sakshith <ysakshith@gmail.com>
 *
 */


#ifndef __PAGING_H__
#define __PAGING_H__

#include <stdint.h>
#include "core/vm.h"

struct vm;

void mmu_init();

void load_vttbr_el2(uint64_t vmid, uint64_t vttbr);
void load_ttbr0_el2(uint64_t __page_dir_start);
void load_tcr_el2();
void load_vtcr_el2();
void load_mair_el2();
void clear_el1_tlb();
void enable_mmu();
void disable_mmu();
void prepare_page_tables_and_map_memory(uint64_t __page_dir_start);
void map_page(struct vm* _vm, uint64_t phys, uint64_t virt, uint64_t flags);

uint64_t ipa_to_phys(struct vm* vm, uint64_t ipa);

struct vaddr_space* create_virtual_address_space();
int8_t map_virtual_address_space(struct vm* _vm);

struct vaddr_space{
    uint64_t* lv1_table;
};

#endif
