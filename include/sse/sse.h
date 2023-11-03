#ifndef __SSE_H__
#define __SSE_H__

#include <stddef.h>
#include <stdint.h>

#define SSE_FOPEN_HVC_NR        0
#define SSE_FREAD_HVC_NR        1
#define SSE_FWRITE_HVC_NR       2
#define SSE_FSEEK_HVC_NR        3
#define SSE_REWIND_HVC_NR       4
#define SSE_FCLOSE_HVC_NR       5

void sse_hvc_main_handler(uint16_t hvc_number);

int  sse_hvc_fopen_handler(const char* filename, const char* mode);
int  sse_hvc_fread_handler(char* buf, size_t size, size_t nmemb, int fd);
int  sse_hvc_fwrite_handler(char* buf, size_t size, size_t nmemb, int fd);
int  sse_hvc_fseek_handler(int fd, uint64_t offset, int pos);
int  sse_hvc_rewind_handler(int fd);
int  sse_hvc_fclose_handler(int fd);

#endif