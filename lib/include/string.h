 /*
 * Copyright (c) 2023 Yuvraj Sakshith <ysakshith@gmail.com>
 *
 */

#ifndef __STRING_H__
#define __STRING_H__

#include <stdint.h>

uint32_t strlen(const char* str);
char* strcpy(char* tgt, const char* src);
int strncmp(const char* s1, const char* s2, uint32_t n);
int strncpy(char* tgt, char* src, uint32_t len);
char* strchr(const char*, int);
int strcmp(const char* s1, const char* s2);

#endif
