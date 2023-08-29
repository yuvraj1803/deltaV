#include "mm/mm.h"
#include "memory.h"
#include <stdint.h>
#include "stdio.h"

#define PAGE_FREE		0b0000
#define PAGE_TAKEN		0b0010
#define PAGE_HAS_NEXT		0b0100
#define PAGE_IS_FIRST		0b1000


uint64_t heap_entries[TOTAL_PAGES];

void heap_init(){
	memset(&heap_entries, PAGE_FREE, sizeof(heap_entries));
}


void* malloc(uint64_t size){
	return 0;	
}

void free(void* base){
	
}


uint64_t mm_r(uint64_t reg){
	return *(volatile uint64_t*)reg;
}

void mm_w(uint64_t reg, uint64_t val){
	*(volatile uint64_t*)reg = val;
}
