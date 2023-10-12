#ifndef __CONSOLE_H__
#define __CONSOLE_H__

struct vm_console{
	int in;
	int out;
	char buffer[CONFIG_VM_CONSOLE_BUFFER_SIZE];
};


#endif