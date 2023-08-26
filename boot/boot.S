#include "config.h"

.section ".text.boot"

.globl _start

_start:
	bl cpu_check 		// we check if we are on the main CPU or not. Otherwise, we just halt
	bl bss_init 		// initialise bss section
			
	mrs x0, currentel       // we need to check which EL we are in
	lsr x0, x0, 2

	cmp x0, 3		// if we are in EL3, jump to el3_entry
	beq el3_entry

	cmp x0, 2		// if we are in EL2, jump to el2_entry
	beq el2_entry

cpu_check:
	mrs x0, mpidr_el1
	and x0, x0, 0b11
	cbnz x0, halt
	ret

bss_init:	
	ldr x0, =__bss_start
	ldr x1, =__bss_end
	ldr x2, =0

1:	str x2, [x0]
	add x0, x0, 1
	cmp x0, x1
	ble 1b
	ret

el3_entry:
	ldr x1,=EL3_STACK_BASE 	// setup EL3 stack
	mov sp, x1

	ldr x1, =EL3_VECTOR_TABLE	// setup EL3 vector table
	msr vbar_el3 ,x1


	ldr x1, =EL3_SCR		//setup SCR in EL3
	msr scr_el3, x1

	ldr x1, =EL3_SPSR		// setup SPSR in EL3
	msr spsr_el3, x1

	adr x1, el2_entry
	msr elr_el3, x1
	eret				// jump to el2
	


el2_entry:
	ldr x1, =EL2_STACK_BASE
	mov sp, x1

//	ldr x1, =EL2_HCR		// setup HCR in EL2
//	msr hcr_el2, x1


//        ldr x1, =EL2_SPSR               // setup SPSR in EL2
//        msr spsr_el2, x1

	bl delta_main
	b halt


halt:
	wfe
	b halt 
	
