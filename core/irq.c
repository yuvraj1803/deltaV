#include "core/irq.h"
#include "mm/mm.h"
#include <stdint.h>
#include "stdio.h"


const char* exception_info[] = {
	"CURRENT_EL_SP0_SYNC",
	"CURRENT_EL_SP0_IRQ",
	"CURRENT_EL_SP0_FIQ",
	"CURRENT_EL_SP0_SERROR",
	
	"CURRENT_EL_SPX_SYNC",
	"CURRENT_EL_SPX_IRQ",
	"CURRENT_EL_SPX_FIQ",
	"CURRENT_EL_SPX_SERROR",
	
	"LOWER_EL_A64_SYNC",
	"LOWER_EL_A64_IRQ",
	"LOWER_EL_A64_FIQ",
	"LOWER_EL_A64_SERROR",
	
	"LOWER_EL_A32_SYNC",
	"LOWER_EL_A32_IRQ",
	"LOWER_EL_A32_FIQ",
	"LOWER_EL_A32_SERROR"
};

// refer to bcm2835 reference manual for more details about these constants

#define ENABLE_IRQs_1		(PERIPH_BASE + 0xB210)
#define ENABLE_IRQs_2		(PERIPH_BASE + 0xB214)
#define SYSTEM_TIMER_MATCH_1	(1U << 1)
#define SYSTEM_TIMER_MATCH_3	(1U << 3)
#define AUX_INT			(1U << 29)



void interrupt_controller_init(){
	mm_w(ENABLE_IRQs_1, mm_r(ENABLE_IRQs_1) | SYSTEM_TIMER_MATCH_1);
	mm_w(ENABLE_IRQs_1, mm_r(ENABLE_IRQs_1) | SYSTEM_TIMER_MATCH_3);
        mm_w(ENABLE_IRQs_1, mm_r(ENABLE_IRQs_1) | AUX_INT);
}

void log_unsupported_exception(uint64_t exception_type,
			       uint64_t esr_el2,
			       uint64_t elr_el2,
			       uint64_t far_el2){

	printf("[UNCAUGHT EXCEPTION] : %s [ESR] : %d [ELR] : %d [FAR] : %d \n", exception_info[exception_type], esr_el2, elr_el2, far_el2);
}


void handle_irq(){

}
void handle_sync(uint64_t esr_el2, uint64_t elr_el2, uint64_t far_el2, uint64_t hvc_number){

}
