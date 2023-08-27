#include "stdio.h"
#include "drivers/uart.h"
#include "drivers/sd.h"

void delta_main(void){
	
	uart_init(); //log("UART initialised");
	if(sd_init() == SD_OK) log("SD initialised");
	else{
		panic("SD initialisation failed!");
	}
	unsigned char buf[100];
	sd_readblock(0,buf, 1);

	while(1) log((char*)buf);
//	while(1) log("hello");

	while(1);
}
