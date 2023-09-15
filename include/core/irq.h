#ifndef __IRQ_H__
#define __IRQ_H__ 

#include <stdint.h>


void irq_init();
void __disable_irq();
void __enable_irq();
void handle_irq();
void handle_sync(uint64_t esr_el2, uint64_t elr_el2, uint64_t far_el2, uint64_t hvc_number);
void log_unsupported_interrupt(uint64_t interrupt_type, uint64_t esr_el2, uint64_t elr_el2, uint64_t far_el2);


#endif
