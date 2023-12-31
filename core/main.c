/*
 *
 * Copyright (c) 2023 Yuvraj Sakshith <ysakshith@gmail.com>
 *
 */


#include "stdio.h"
#include "drivers/uart.h"
#include "drivers/sd.h"
#include "drivers/timer.h"
#include "mm/mm.h"
#include "fs/diskio.h"
#include "fs/ff.h"
#include "core/irq.h"
#include "core/misc.h"
#include "core/sched.h"
#include "core/vm.h"
#include "shell/shell.h"
#include "sse/sse.h"


void main(void){
	
	__disable_irq();
	
	mmu_init();
	uart_init();
	heap_init();
	fs_init();
	irq_init();
	interrupt_controller_init();
	timer_init();
	sched_init();
	shell_init();

	
	vm_init("/guests/deltaOS/kernel8.img", 0x0,0x80000,0x80000);
	vm_init("/guests/deltaOS/kernel8.img", 0x0,0x80000,0x80000);

	
	sse_init();		// Secure Storage Enclave


	printf("\nPress Enter for Shell\n\n");

	while(1){
		__disable_irq();
		schedule();
		__enable_irq();
	}
}
