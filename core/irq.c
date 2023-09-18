#include "core/irq.h"
#include "mm/mm.h"
#include <stdint.h>
#include "stdio.h"
#include "drivers/timer.h"
#include "core/misc.h"

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

#define INTERRUPT_CONTROLLER_BASE	(PERIPH_BASE + 0xB000)
#define ENABLE_IRQs_1			(INTERRUPT_CONTROLLER_BASE + 0x210)
#define ENABLE_IRQs_2			(INTERRUPT_CONTROLLER_BASE + 0x214)
#define DISABLE_IRQs_1			(INTERRUPT_CONTROLLER_BASE + 0x21C)
#define DISABLE_IRQs_2			(INTERRUPT_CONTROLLER_BASE + 0x220)
#define IRQ_PENDING_1			(INTERRUPT_CONTROLLER_BASE + 0x204)
#define IRQ_PENDING_2			(INTERRUPT_CONTROLLER_BASE + 0x208)
#define FIQ_CONTROL			(INTERRUPT_CONTROLLER_BASE + 0x20C)
#define SYSTEM_TIMER_MATCH_1		(1U << 1)
#define SYSTEM_TIMER_MATCH_3		(1U << 3)
#define AUX_INT				(1U << 29)
#define FIQ_ENABLE			(1U << 7)


void interrupt_controller_init(){
	mm_w32(FIQ_CONTROL, 0xc1);
	mm_w32(ENABLE_IRQs_1, SYSTEM_TIMER_MATCH_1);
	mm_w32(ENABLE_IRQs_1, SYSTEM_TIMER_MATCH_3);
        mm_w32(ENABLE_IRQs_1, AUX_INT);
}

void log_unsupported_exception(uint64_t exception_type,
			       uint64_t esr_el2,
			       uint64_t elr_el2,
			       uint64_t far_el2){

	printf("[UNCAUGHT EXCEPTION] : %s [ESR] : %x [ELR] : %x [FAR] : %x [EL] : %d\n", exception_info[exception_type], esr_el2, elr_el2, far_el2, get_current_el());
}


void handle_irq(){
	uint32_t pending = mm_r(IRQ_PENDING_1);
	if(pending & SYSTEM_TIMER_MATCH_1) system_timer_1_handler();
	if(pending & SYSTEM_TIMER_MATCH_3) system_timer_3_handler();

}
void handle_sync(uint64_t esr_el2, uint64_t elr_el2, uint64_t far_el2, uint64_t hvc_number){
}
