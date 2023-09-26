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



void delta_main(void){
	
	__disable_irq();
	
	mmu_init();
	uart_init();
	heap_init();
	fs_init();
	irq_init();
	interrupt_controller_init();
	timer_init();
	sched_init();

	vm_init("/guests/hw.bin", 0x80000,0x80000,0x80000);


	shell_init();

	while(1){
		__disable_irq();
		schedule();
		__enable_irq();
	}
}
