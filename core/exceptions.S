#include "core/exceptions.h"


.macro hypervisor_entry
		// we pull back sp by 8*34 addresses because 34 registers needs to be pushed
		sub sp, sp, #8*34
        stp x0, x1, [sp, #16 * 0]
        stp x2, x3, [sp, #16 * 1]
        stp x4, x5, [sp, #16 * 2]
        stp x6, x7, [sp, #16 * 3]
        stp x8, x9, [sp, #16 * 4]
        stp x10, x11, [sp, #16 * 5]
        stp x12, x13, [sp, #16 * 6]
        stp x14, x15, [sp, #16 * 7]
        stp x16, x17, [sp, #16 * 8]
        stp x18, x19, [sp, #16 * 9]
        stp x20, x21, [sp, #16 * 10]
        stp x22, x23, [sp, #16 * 11]
        stp x24, x25, [sp, #16 * 12]
        stp x26, x27, [sp, #16 * 13]
        stp x28, x29, [sp, #16 * 14]
        add x21, sp, #8*34
        mrs x22, elr_el2
        mrs x23, spsr_el2
        stp x30, x21, [sp, #16 * 15]
        stp x22, x23, [sp, #16 * 16]
.endm

.macro hypervisor_exit
        ldp x30, x21, [sp, #16 * 15]
        ldp x22, x23, [sp, #16 * 16]
        msr elr_el2, x22
        msr spsr_el2, x23
        ldp x0, x1, [sp, #16 * 0]
        ldp x2, x3, [sp, #16 * 1]
        ldp x4, x5, [sp, #16 * 2]
        ldp x6, x7, [sp, #16 * 3]
        ldp x8, x9, [sp, #16 * 4]
        ldp x10, x11, [sp, #16 * 5]
        ldp x12, x13, [sp, #16 * 6]
        ldp x14, x15, [sp, #16 * 7]
        ldp x16, x17, [sp, #16 * 8]
        ldp x18, x19, [sp, #16 * 9]
        ldp x20, x21, [sp, #16 * 10]
        ldp x22, x23, [sp, #16 * 11]
        ldp x24, x25, [sp, #16 * 12]
        ldp x26, x27, [sp, #16 * 13]
        ldp x28, x29, [sp, #16 * 14]
        add sp, sp, #8*34
        eret
.endm

.globl __prepare_vm
__prepare_vm:
	bl prepare_vm
	hypervisor_exit

.macro handle_unsupported_interrupt interrupt_type
	hypervisor_entry
	mov x0, #\interrupt_type
	mrs x1, esr_el2
	mrs x2, elr_el2
	mrs x3, far_el2
	bl log_unsupported_exception
	b halt 
.endm

.macro handle_irq_interrupt
	hypervisor_entry
	bl handle_irq
	hypervisor_exit
	eret
.endm

.macro handle_sync_interrupt
	hypervisor_entry
	mrs x0, esr_el2
	mrs x1, elr_el2
	mrs x2, far_el2
	mov x3, x8 	// hvc number
	bl handle_sync
	hypervisor_exit
	eret
.endm

.macro ventry handler
.align 7
b \handler
.endm


.align 11
.globl vtable
vtable:
	ventry current_el_sp0_sync
	ventry current_el_sp0_irq
	ventry current_el_sp0_fiq
	ventry current_el_sp0_serror

	ventry current_el_spx_sync
	ventry current_el_spx_irq
	ventry current_el_spx_fiq
	ventry current_el_spx_serror

	ventry lower_el_a64_sync
	ventry lower_el_a64_irq
	ventry lower_el_a64_fiq
	ventry lower_el_a64_serror
	
	ventry lower_el_a32_sync
	ventry lower_el_a32_irq
	ventry lower_el_a32_fiq
	ventry lower_el_a32_serror

 current_el_sp0_sync:
 	handle_unsupported_interrupt CURRENT_EL_SP0_SYNC 	
 current_el_sp0_irq:
  	handle_unsupported_interrupt CURRENT_EL_SP0_IRQ
 current_el_sp0_fiq:
  	handle_unsupported_interrupt CURRENT_EL_SP0_FIQ
 current_el_sp0_serror:
  	handle_unsupported_interrupt CURRENT_EL_SP0_SERROR

 current_el_spx_sync:
  	handle_unsupported_interrupt CURRENT_EL_SPX_SYNC
 current_el_spx_irq:
  	handle_irq_interrupt 
 current_el_spx_fiq:
  	handle_unsupported_interrupt CURRENT_EL_SPX_FIQ
 current_el_spx_serror:
  	handle_unsupported_interrupt CURRENT_EL_SPX_SERROR

 lower_el_a64_sync:
  	handle_sync_interrupt 
 lower_el_a64_irq:
  	handle_irq_interrupt
 lower_el_a64_fiq:
  	handle_unsupported_interrupt LOWER_EL_A64_FIQ
 lower_el_a64_serror:
  	handle_unsupported_interrupt LOWER_EL_A64_SERROR

 lower_el_a32_sync:
  	handle_unsupported_interrupt LOWER_EL_A32_SYNC
 lower_el_a32_irq:
  	handle_unsupported_interrupt LOWER_EL_A32_IRQ
 lower_el_a32_fiq:
  	handle_unsupported_interrupt LOWER_EL_A32_FIQ
 lower_el_a32_serror:
  	handle_unsupported_interrupt LOWER_EL_A32_SERROR









