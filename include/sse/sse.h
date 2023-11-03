#ifndef __SSE_H__
#define __SSE_H__

#include <stddef.h>
#include <stdint.h>

#define SSE_FREAD_HVC_NR        0
#define SSE_FWRITE_HVC_NR       1

void sse_init();
void sse_hvc_main_handler(uint16_t hvc_number);

int  sse_hvc_fread_handler(char* filename, char* buf, size_t size);
int  sse_hvc_fwrite_handler(char* filename, char* buf, size_t size);
#endif