/*
 *
 * Copyright (c) 2023 Yuvraj Sakshith <ysakshith@gmail.com>
 *
 */


#include "stdio.h"
#include "stdlib.h"
#include "drivers/uart.h"
#include <stdarg.h>

void panic(char *str){
	uart_write("PANIC: ");
	uart_write(str);
	uart_write("\n");
	while(1);
}

char getch(){
	return uart_rx();
}

void gets(char* str){
	int index = 0;
	char ch;

	while(1){
		ch = uart_rx();
		if(ch == '\n'){
			uart_tx('\n');
			break;
		}
		if(ch == 127) continue; // backspace
		str[index] = ch;
		index++;
		uart_tx(ch);
	}

	str[index] = '\0';

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


void log(char *str){
	uart_write("LOG: ");
	printf(str);
	uart_write("\n");
}