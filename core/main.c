#include "drivers/uart/uart.h"

void delta_main(void){
	
	uart_init();
	uart_write("yuvraj here");

	while(1);
}
