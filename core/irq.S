.globl __disable_irq
__disable_irq:
	msr daifset, #2
	ret

.globl __enable_irq
__enable_irq:
	msr daifclr, #2
	ret
