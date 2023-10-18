.globl __disable_irq
.globl __enable_irq
.globl irq_init
.globl assert_virtual_irq
.globl clear_virtual_irq
.globl assert_virtual_fiq
.globl clear_virtual_fiq


__disable_irq:
	msr daifset, #2
	ret

__enable_irq:
	msr daifclr, #2
	ret


irq_init:
	adr x0, vtable
	msr vbar_el2, x0
	ret

assert_virtual_irq:
	mrs x0, hcr_el2
	orr x1, x0, #0x80	// VI bit
	msr hcr_el2, x1
	ret

clear_virtual_irq:
	mrs x0, hcr_el2
	bic x1, x0, #0x80	// VI bit
	msr hcr_el2, x1
	ret

assert_virtual_fiq:
	mrs x0, hcr_el2
	orr x1, x0, #0x40	// VF bit
	msr hcr_el2, x1
	ret

clear_virtual_fiq:
	mrs x0, hcr_el2
	bic x1, x0, #0x40	// VF bit
	msr hcr_el2, x1
	ret


