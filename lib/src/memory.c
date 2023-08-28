/*
 *
 * Copyright (c) 2023 Yuvraj Sakshith <ysakshith@gmail.com>
 *
 */

#include "memory.h"

// starting from the base address, sets 'size' number of bytes to value provided.
void * memset(void * base_addr, int init_val, size_t size){
    char * __base_addr = (char*) base_addr;

    for(int i=0;i<size;i++){
        __base_addr[i] = (char)init_val;
    }

    return (void*)__base_addr;
}


// copies _size bytes of memory from src to tgt.
void* memcpy(void* tgt, const void* src, size_t _size){
    uint8_t* tgt_ptr = (uint8_t *) tgt;
    uint8_t* src_ptr = (uint8_t *) src;

    while(_size--){
        *tgt_ptr = *src_ptr;
        tgt_ptr++;
        src_ptr++;
    }

    return tgt;

}

// compare first 'size' bytes of buf1 and buf2
// returns 0 if they are the same
uint8_t memcmp(void* buf1, void* buf2, uint32_t size){
    uint8_t* buf1_ptr = (uint8_t*) buf1;
    uint8_t* buf2_ptr = (uint8_t*) buf2;

    while(size-- && buf1_ptr != 0 && buf2_ptr != 0){
        if(*buf1_ptr != *buf2_ptr) return 1;

        buf1_ptr++;
        buf2_ptr++;
    }

    return 0;

}
