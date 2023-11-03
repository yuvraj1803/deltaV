/*
 *
 * Copyright (c) 2023 Yuvraj Sakshith <ysakshith@gmail.com>
 *
 */


#include "mm/paging.h"
#include "core/vm.h"
#include "stdio.h"
#include "kstatus.h"
#include "mm/mmu.h"
#include "mm/mm.h"

#define LV1_SHIFT		30
#define LV2_SHIFT   	21
#define PAGE_SHIFT		12
#define TABLE_SHIFT		9
#define TABLE_ENTRIES	(1U << TABLE_SHIFT)

extern volatile uint64_t __page_dir_start;

struct vaddr_space* create_virtual_address_space(){
	uint64_t* lv1_table = (uint64_t*) malloc(PAGE_SIZE); // 1GB blocks	

	struct vaddr_space *new_virtual_address_space = (struct vaddr_space*) malloc(sizeof(struct vaddr_space));
	new_virtual_address_space->lv1_table = lv1_table;

	return new_virtual_address_space;
}

static uint64_t map_table(struct vm* _vm, uint64_t table, uint64_t shift, uint64_t virt){
	uint64_t table_index = (virt >> shift) & (TABLE_ENTRIES-1);

	if(!((uint64_t*)table)[table_index]){
		uint64_t* next_table = malloc(PAGE_SIZE);
		((uint64_t*)table)[table_index] = (uint64_t)next_table | MMU_TABLE_ENTRY_FLAG;
	}

	return ((uint64_t*)table)[table_index] & ~((1U << PAGE_SHIFT)-1);

}

void map_page(struct vm* _vm, uint64_t phys, uint64_t virt, uint64_t flags){

	uint64_t lv1_table = (uint64_t)_vm->virtual_address_space->lv1_table;
	uint64_t lv2_table = map_table(_vm, lv1_table, LV1_SHIFT, virt);
	uint64_t lv3_table = map_table(_vm, lv2_table, LV2_SHIFT, virt);
	
	uint64_t lv3_index = (virt >> PAGE_SHIFT) & (TABLE_ENTRIES-1);
	
	((uint64_t*) lv3_table)[lv3_index] = phys | flags;
}

int8_t map_virtual_address_space(struct vm* _vm){
	uint64_t virt = (uint64_t)_vm->load_addr;
	uint64_t phys = (uint64_t)_vm->vmdata;
	uint64_t pages = (_vm->vmdata_size + PAGE_SIZE - 1) / PAGE_SIZE;

	struct vaddr_space* virtual_address_space = _vm->virtual_address_space;
	if(!virtual_address_space) return -ERR_VM_VMAPPING_FAIL;

	while(pages > 0){

		map_page(_vm, phys, virt, MMU_STAGE_2_MEM_FLAGS);	// we map one page at a time.

		phys += PAGE_SIZE;
		virt += PAGE_SIZE;
		pages--;
	}

	uint64_t mmio_base = 0x3f000000;
	uint64_t mmio_end  = 0x40000000 - 0x200000;


	// identity mapping the MMIO region with NO ACCESS flags. check include/mm/mmu.h
	for(int phys=mmio_base; phys <= mmio_end; phys += PAGE_SIZE){
		map_page(_vm, phys, phys, MMU_STAGE_2_MMIO_FLAGS); 
	}

	return SUCCESS;
}

uint64_t ipa_to_phys(struct vm* vm,	uint64_t ipa){ 	// returns physical page base addr when given IPA of given VM.

	uint64_t ipa_page_offset = ipa & (PAGE_SIZE - 1);
	uint64_t ipa_page		 = ipa &~(PAGE_SIZE - 1);

	uint64_t lv1_table = vm->virtual_address_space->lv1_table;	// 512 GB block
	
	uint64_t lv2_table = ((uint64_t*)lv1_table)[0] & ~(PAGE_SIZE - 1);				// 1 GB block
	uint64_t lv2_index = (ipa_page >> LV2_SHIFT) & (TABLE_ENTRIES - 1);

	uint64_t lv3_table = ((uint64_t*)lv2_table)[lv2_index] & ~(PAGE_SIZE - 1);
	uint64_t lv3_index = (ipa_page >> PAGE_SHIFT) & (TABLE_ENTRIES - 1);


	return (((uint64_t*)lv3_table)[lv3_index] & ~(PAGE_SIZE-1)) + ipa_page_offset;

}

void mmu_init(){

	prepare_page_tables_and_map_memory((uint64_t)&__page_dir_start);
	load_ttbr0_el2(((uint64_t)(uint64_t*)&__page_dir_start));
	load_tcr_el2();
	load_vtcr_el2();
	load_mair_el2();
	clear_el1_tlb();
	enable_mmu();
	
	log("MMU initialised");
}
