#include "core/irq.h"
#include <stdint.h>

void log_unsupported_interrupt(uint64_t interrupt_type,
			       uint64_t esr_el2,
			       uint64_t elr_el2,
			       uint64_t far_el2){
}

void handle_irq(){

}
void handle_sync(uint64_t esr_el2, uint64_t elr_el2, uint64_t far_el2, uint64_t hvc_number){

}
