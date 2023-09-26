#include "mm/paging.h"
#include "core/vm.h"
#include "stdio.h"
#include "kstatus.h"
#include "mm/mmu.h"

#define LV1_SHIFT	21
#define LV2_SHIFT   12
#define PAGE_SHIFT	12
#define TABLE_SHIFT	9

extern volatile uint32_t __page_dir_start;

struct vaddr_space* create_virtual_address_space(){
	uint64_t* lv1_table = malloc(PAGE_SIZE); // 1GB blocks	

	struct vaddr_space *new_virtual_address_space = (struct vaddr_space*) malloc(sizeof(struct vaddr_space));
	new_virtual_address_space->lv1_table = lv1_table;

	return new_virtual_address_space;
}

static void map_page(struct vm* _vm, uint64_t phys, uint64_t virt, uint64_t flags){

	uint64_t* lv1_table = _vm->virtual_address_space->lv1_table;
	uint64_t lv1_table_index = (virt >> LV1_SHIFT) & ((1U << TABLE_SHIFT) - 1);

	if(lv1_table[lv1_table_index] == 0){ // ie. if this 2MB block has never been mapped before
		uint64_t* lv2_table = malloc(PAGE_SIZE);
		uint64_t lv2_table_index = (virt >> LV2_SHIFT) & ((1U << TABLE_SHIFT) - 1);

		lv1_table[lv1_table_index] = (uint64_t) lv2_table | MMU_DESCRIPTOR_TABLE_DESCRIPTOR_FLAG;
		lv2_table[lv2_table_index] = phys | flags;
	}else{
		uint64_t* lv2_table = lv1_table[lv1_table_index] & (~MMU_DESCRIPTOR_TABLE_DESCRIPTOR_FLAG);
		uint64_t lv2_table_index = (virt >> LV2_SHIFT) & ((1U << TABLE_SHIFT) - 1);

		lv2_table[lv2_table_index] = phys | flags;
	}


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

void mmu_init(){

	prepare_page_tables_and_map_memory(&__page_dir_start);
	load_ttbr0_el2(&__page_dir_start);
	load_tcr_el2();
	load_vtcr_el2();
	load_mair_el2();
	clear_el1_tlb();
	enable_mmu();
	
	log("MMU initialised");
}
