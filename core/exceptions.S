#include "core/exceptions.h"

.macro handle_unsupported_interrupt interrupt_type
//	switch_to_hypervisor
	mov x0, #\interrupt_type
	mrs x1, esr_el2
	mrs x2, elr_el2
	mrs x3, far_el2
	bl log_unsupported_interrupt
	b halt 
.endm

.macro handle_irq_interrupt
// 	switch_to_hypervisor
	bl handle_irq
//	switch_to_vm
.endm

.macro handle_sync_interrupt
//	switch_to_hypervisor
	mrs x0, esr_el2
	mrs x1, elr_el2
	mrs x2, far_el2
	mov x3, x8 	// hvc number
	bl handle_sync
//	switch_to_vm
.endm

.macro interrupt_vector_table_entry handler
.align 7
b \handler
.endm


.align 11
.globl interrupt_vector_table
interrupt_vector_table:
	interrupt_vector_table_entry current_el_sp0_sync
	interrupt_vector_table_entry current_el_sp0_irq
	interrupt_vector_table_entry current_el_sp0_fiq
	interrupt_vector_table_entry current_el_sp0_serror

	interrupt_vector_table_entry current_el_spx_sync
	interrupt_vector_table_entry current_el_spx_irq
	interrupt_vector_table_entry current_el_spx_fiq
	interrupt_vector_table_entry current_el_spx_serror

	interrupt_vector_table_entry lower_el_a64_sync
	interrupt_vector_table_entry lower_el_a64_irq
	interrupt_vector_table_entry lower_el_a64_fiq
	interrupt_vector_table_entry lower_el_a64_serror
	
	interrupt_vector_table_entry lower_el_a32_sync
	interrupt_vector_table_entry lower_el_a32_irq
	interrupt_vector_table_entry lower_el_a32_fiq
	interrupt_vector_table_entry lower_el_a32_serror

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









