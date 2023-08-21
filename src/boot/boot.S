.section ".boot.text"

.globl _start

_start:
	bl cpu_check
	bl deltaV_start

cpu_check:
	mrs x0, mpidr_el1
	and x0, x0, 0b11
	cbnz x0, halt
	ret

halt:
	wfe
	b halt 
	
