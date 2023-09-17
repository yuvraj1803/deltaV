#include "stdio.h"
#include "drivers/uart.h"
#include "drivers/sd.h"
#include "mm/mm.h"
#include "fs/diskio.h"
#include "fs/ff.h"
#include "core/irq.h"


void delta_main(void){
	
	uart_init();
	heap_init();
	fs_init();
	irq_init();
	interrupt_controller_init();
	
	printf("hello world %s", "yuvrj");
	while(1);

	unsigned char buf[100];
	FIL fp;
	FRESULT r;
	r = f_open(&fp, "/guests/hw.bin", FA_READ);
	if(r){
		log("unable to open file");
		while(1);
	}
	f_read(&fp, buf, 100, 0);

	log(buf);
	
	while(1);
}
