.section ".boot.text"

.globl _start

_start:
	bl cpu_check 		// we check if we are on the main CPU or not. Otherwise, we just halt
	bl main

cpu_check:
	mrs x0, mpidr_el1
	and x0, x0, 0b11
	cbnz x0, halt
	ret

halt:
	wfe
	b halt 
	
