/*
 *
 * Copyright (c) 2023 Yuvraj Sakshith <ysakshith@gmail.com>
 *
 */


#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "config.h"

struct vm_console{
	int begin;
	int end;
	char buffer[CONFIG_VM_CONSOLE_BUFFER_SIZE];
	int used;
};

void console_push(struct vm_console*, int val);
int console_pop(struct vm_console*);
int console_empty(struct vm_console*);
void console_clear(struct vm_console*);
int console_full(struct vm_console* console);
void console_flush(struct vm_console* console);

#endif