/*
 *
 * Copyright (c) 2023 Yuvraj Sakshith <ysakshith@gmail.com>
 *
 */


#include "mm/mm.h"
#include "memory.h"
#include <stdint.h>
#include "stdio.h"
#include "kstatus.h"

#define PAGE_FREE		0b0001
#define PAGE_TAKEN		0b0010
#define PAGE_HAS_NEXT		0b0100
#define PAGE_IS_FIRST		0b1000


uint64_t __attribute__((section(".page_map"))) page_map[TOTAL_PAGES];

void heap_init(){
	for(int i=0;i<TOTAL_PAGES;i++) page_map[i] = PAGE_FREE;
	log("HEAP initialised");
}


void malloc_fill_blocks_taken(uint64_t start_block, uint64_t total_blocks){
	
	for(int block=start_block; block < start_block + total_blocks; block++){
		if(block == start_block){
			page_map[block] = PAGE_TAKEN | PAGE_IS_FIRST;	// first block
		}else if(block == start_block + total_blocks - 1){
			page_map[block] = PAGE_TAKEN;			// last block
		}else{
			page_map[block] = PAGE_TAKEN | PAGE_HAS_NEXT;	// middle block
		}
	}	

}

void malloc_fill_blocks_free(uint64_t start_block){
	for(int block = start_block;; block++){
		if(page_map[block] == PAGE_TAKEN){
			page_map[block] = PAGE_FREE;
			return;
		}
		if(page_map[block] == PAGE_FREE) return;

		page_map[block] = PAGE_FREE;
	}
}

uint64_t malloc_find_free_block(uint64_t pages_to_allocate){
	
	int block = 0;
	while(block < TOTAL_PAGES){
		int block_found = 1;
		int i;
		for(i=block;i<block + pages_to_allocate;i++){
			if(page_map[i] != PAGE_FREE){
				block_found = 0;
				break;
			}
		}
		if(block_found){
			malloc_fill_blocks_taken(block, pages_to_allocate);
			return block;
		}
		block = i+1;
	}

	return -ERR_MALLOC_FAIL;
}


void* malloc(uint64_t size){
	
	size = size + (PAGE_SIZE - size % PAGE_SIZE);	// ALIGN SIZE REQUESTED TO PAGE SIZE ABOVE
	uint32_t pages_to_allocate = size / PAGE_SIZE;
	
	uint64_t block_allocated = malloc_find_free_block(pages_to_allocate);
	
	if(block_allocated < 0){ // couldnt find large enough contagious memory 
		return NULL;
	}

	return (void*)(HEAP_LOW + block_allocated * PAGE_SIZE); 

}

void free(void* base){
	uint64_t base_address = (uint64_t) base;
	uint64_t base_address_page_aligned_to_lower = base_address - base_address%PAGE_SIZE;
	uint64_t base_address_page_number = (base_address_page_aligned_to_lower - HEAP_LOW) / PAGE_SIZE;

	malloc_fill_blocks_free(base_address_page_number);
}


uint64_t mm_r(uint64_t reg){
	return *(volatile uint64_t*)reg;
}

void mm_w(uint64_t reg, uint64_t val){
	*(volatile uint64_t*)reg = val;
}
