#include "stdio.h"
#include "drivers/uart.h"
#include "drivers/sd.h"

void delta_main(void){
	
	uart_init();
	sd_init();
//	while(1) log("hello");

	while(1);
}
