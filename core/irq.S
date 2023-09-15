.globl __disable_irq
.globl __enable_irq
.globl irq_init


__disable_irq:
	msr daifset, #2
	ret

__enable_irq:
	msr daifclr, #2
	ret


irq_init:
	adr x0, interrupt_vector_table
	msr vbar_el2, x0
	ret



