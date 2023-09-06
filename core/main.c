#include "stdio.h"
#include "drivers/uart.h"
#include "drivers/sd.h"
#include "mm/mm.h"
#include "fs/diskio.h"
#include "fs/ff.h"

FATFS fatfs;

void delta_main(void){
	
	uart_init();
	heap_init();

	f_mount(&fatfs, "/",0);
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
