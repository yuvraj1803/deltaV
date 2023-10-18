#ifndef __IRQ_H__
#define __IRQ_H__ 

#include <stdint.h>


void irq_init();
void __disable_irq();
void __enable_irq();
void handle_irq();
void log_unsupported_exception(uint64_t exception_type, uint64_t esr_el2, uint64_t elr_el2, uint64_t far_el2);
void interrupt_controller_init();
void assert_virtual_irq();
void clear_virtual_irq();
void assert_virtual_fiq();
void clear_virtual_fiq();

#endif
