#include "drivers/uart.h"
#include "mm/mm.h"
#include "stdio.h"
#include <stdint.h>

void uart_init(){
	
	mm_w(AUX_ENABLES, mm_r(AUX_ENABLES) | 0x1);
	mm_w(AUX_MU_IER_REG, 0);
	mm_w(AUX_MU_CNTL_REG,0);
	mm_w(AUX_MU_LCR_REG, 3); // uart 8-bit mode
	mm_w(AUX_MU_MCR_REG, 0);
	mm_w(AUX_MU_IER_REG, 0);
	mm_w(AUX_MU_IIR_REG, 0xc6);
	mm_w(AUX_MU_BAUD_REG, 270); // 115200 baudrate
	mm_w(GPFSEL1, mm_r(GPFSEL1)& ~((0b11 << 12 | 0b11 << 15))); // reset GPIO Pins 14 and 15
	mm_w(GPFSEL1, mm_r(GPFSEL1) | (0b10 << 14 | 0b10 << 15));

	mm_w(GPPUD, 0);
	for(int i=0;i<150;i++); // wait for 150 cycles
	mm_w(GPPUDCLK0, ((1u << 14) | (1u << 15)));
	for(int i=0;i<150;i++); // wait for 150 cycles
	mm_w(GPPUDCLK0, 0);

	mm_w(AUX_MU_CNTL_REG, 3);  //enable tx/rx
				   //
	log("UART initialised");

}

void uart_tx(char ch){
	do{
		asm volatile("nop");
	}while(!(mm_r(AUX_MU_LSR_REG) & 0x20)); // wait for UART tx

	mm_w(AUX_MU_IO_REG, ch);
}

char uart_rx(){
	do{
		asm volatile("nop");
	}while(!(mm_r(AUX_MU_LSR_REG) & 0x01)); // wait for something to be written into buf.
	
	if(mm_r(AUX_MU_IO_REG) == '\r') return '\n'; // conv carriage ret into newline

	return mm_r(AUX_MU_IO_REG & 0xFF);
}

void uart_write_size(char * str, uint64_t size){
	char *ptr = str;
	while(size--){
 	       if(*ptr == '\n') *ptr = '\r';

               uart_tx(*ptr);
               ptr++;
	}
}

void uart_write(char* str){
	char* ptr = str;
	while(*ptr){
		if(*ptr == '\n'){
			uart_tx('\r');
		}

		uart_tx(*ptr++);
	}
}

void uart_write_hex(unsigned int x){
    unsigned int n;
    int c;
    for(c=28;c>=0;c-=4) {
        n=(x>>c)&0xF;
        n+=n>9?0x37:0x30;
        uart_tx(n);
    }

}
