#include <stdio.h>
#include "drivers/uart.h"

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
