#include "stdio.h"
#include "stdlib.h"
#include "drivers/uart.h"
#include <stdarg.h>

void log(char *str){
	uart_write("LOG: ");
	uart_write(str);
	uart_write("\n");
}

void panic(char *str){
	uart_write("PANIC: ");
	uart_write(str);
	uart_write("\n");
	while(1);
}


void printf(char* fmt, ...){
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
				default:
					break;
			}
		}
	}
}
