#include "mm/mm.h"

uint64_t mm_r(uint64_t reg){
	return *(volatile uint64_t*)reg;
}

void mm_w(uint64_t reg, uint64_t val){
	*(volatile uint64_t*)reg = val;
}
