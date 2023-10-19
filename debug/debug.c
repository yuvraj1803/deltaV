#include "debug/debug.h"
#include "stdio.h"
#include "core/vm.h"
#include <stdarg.h>
#include "drivers/uart.h"
#include "stdlib.h"

#ifdef DEBUG

extern struct vm* current;

void debug(char* fmt, ...){

    printf("[VMID: %d] ", current->vmid);

	va_list ap;
	va_start(ap, fmt);

	for(char*p = fmt; *p; p++){
		if(*p != '%'){
			uart_tx(*p);
		}else{
			int val;
			switch(*++p){
				case 'x':
					val = va_arg(ap, int);
					uart_write_hex(val);
					break;
				case 's':
					char* str = va_arg(ap, char*);
					uart_write(str);
					break;
				case 'd':
				    val = va_arg(ap, int);
					uart_write(itoa(val));
					break;
				case 'c':
					val = va_arg(ap, int);
					uart_tx(val);
					break;
				default:
					break;
			}
		}
	}
}

#else

void debug(char* fmt, ...){
    return;     // we dont do anything
}
#endif