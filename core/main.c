#include "stdio.h"
#include "drivers/uart.h"
#include "drivers/sd.h"
#include "mm/mm.h"

void delta_main(void){
	
	uart_init();
	sd_init();
	heap_init();
	
	while(1);
}
