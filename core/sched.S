.globl get_sysregs
.globl put_sysregs
.globl switch_context

switch_context:
	mov x2, sp
	stp x19, x20, [x0], #16
	stp x21, x22, [x0], #16
	stp x23, x24, [x0], #16
	stp x25, x26, [x0], #16
	stp x27, x28, [x0], #16
	stp x29, x2, [x0], #16
	str x30, [x0]
	
	ldp x19, x20, [x1], #16        
	ldp x21, x22, [x1], #16
	ldp x23, x24, [x1], #16
	ldp x25, x26, [x1], #16
	ldp x27, x28, [x1], #16
	ldp x29, x9, [x1], #16
	ldr x30, [x1]
	mov sp, x9
	ret

get_sysregs:
    mrs x1, sctlr_el1
	mrs x2, ttbr0_el1
	stp x1, x2, [x0], #16
	mrs x1, ttbr1_el1
	mrs x2, tcr_el1
	stp x1, x2, [x0], #16
	mrs x1, esr_el1
	mrs x2, far_el1
	stp x1, x2, [x0], #16
	mrs x1, afsr0_el1
	mrs x2, afsr1_el1
	stp x1, x2, [x0], #16
	mrs x1, mair_el1
	mrs x2, amair_el1
	stp x1, x2, [x0], #16
	mrs x1, contextidr_el1
	mrs x2, cpacr_el1
	stp x1, x2, [x0], #16
	mrs x1, elr_el1
	mrs x2, fpcr
	stp x1, x2, [x0], #16
	mrs x1, fpsr
	mrs x2, midr_el1
	stp x1, x2, [x0], #16
	mrs x1, mpidr_el1
	mrs x2, par_el1
	stp x1, x2, [x0], #16
	mrs x1, sp_el0
	mrs x2, sp_el1
	stp x1, x2, [x0], #16
	mrs x1, spsr_el1
	mrs x2, tpidr_el0
	stp x1, x2, [x0], #16
	mrs x1, tpidr_el1
	mrs x2, tpidrro_el0
	stp x1, x2, [x0], #16
	mrs x1, vbar_el1
	mrs x2, actlr_el1
	stp x1, x2, [x0], #16
	mrs x1, id_pfr0_el1
	mrs x2, id_pfr1_el1
	stp x1, x2, [x0], #16
	mrs x1, id_mmfr0_el1
	mrs x2, id_mmfr1_el1
	stp x1, x2, [x0], #16
	mrs x1, id_mmfr2_el1
	mrs x2, id_mmfr3_el1
	stp x1, x2, [x0], #16
	mrs x1, id_isar0_el1
	mrs x2, id_isar1_el1
	stp x1, x2, [x0], #16
	mrs x1, id_isar2_el1
	mrs x2, id_isar3_el1
	stp x1, x2, [x0], #16
	mrs x1, id_isar4_el1
	mrs x2, id_isar5_el1
	stp x1, x2, [x0], #16
	mrs x1, mvfr0_el1
	mrs x2, mvfr1_el1
	stp x1, x2, [x0], #16
	mrs x1, mvfr2_el1
	mrs x2, id_aa64pfr0_el1
	stp x1, x2, [x0], #16
	mrs x1, id_aa64pfr1_el1
	mrs x2, id_aa64dfr0_el1
	stp x1, x2, [x0], #16
	mrs x1, id_aa64dfr1_el1
	mrs x2, id_aa64isar0_el1
	stp x1, x2, [x0], #16
	mrs x1, id_aa64isar1_el1
	mrs x2, id_aa64mmfr0_el1
	stp x1, x2, [x0], #16
	mrs x1, id_aa64mmfr1_el1
	mrs x2, id_aa64afr0_el1
	stp x1, x2, [x0], #16
	mrs x1, id_aa64afr1_el1
	mrs x2, ctr_el0
	stp x1, x2, [x0], #16
	mrs x1, ccsidr_el1
	mrs x2, clidr_el1
	stp x1, x2, [x0], #16
	mrs x1, csselr_el1
	mrs x2, aidr_el1
	stp x1, x2, [x0], #16
	mrs x1, revidr_el1
	mrs x2, cntkctl_el1
	stp x1, x2, [x0], #16
	mrs x1, cntp_ctl_el0
	mrs x2, cntp_cval_el0
	stp x1, x2, [x0], #16
	mrs x1, cntp_tval_el0
	mrs x2, cntv_ctl_el0
	stp x1, x2, [x0], #16
	mrs x1, cntv_cval_el0
	mrs x2, cntv_tval_el0
	stp x1, x2, [x0], #16
	ret

put_sysregs:
	ldp x1, x2, [x0], #16
        msr sctlr_el1, x1
	msr ttbr0_el1, x2
	ldp x1, x2, [x0], #16
	msr ttbr1_el1, x1
	msr tcr_el1, x2
	ldp x1, x2, [x0], #16
	msr esr_el1, x1
	msr far_el1, x2
	ldp x1, x2, [x0], #16
	msr afsr0_el1, x1
	msr afsr1_el1, x2
	ldp x1, x2, [x0], #16
	msr mair_el1, x1
	msr amair_el1, x2
	ldp x1, x2, [x0], #16
	msr contextidr_el1, x1
	msr cpacr_el1, x2
	ldp x1, x2, [x0], #16
	msr elr_el1, x1
	msr fpcr, x2
	ldp x1, x2, [x0], #16
	msr fpsr, x1
	msr vpidr_el2, x2
	ldp x1, x2, [x0], #16
	msr vmpidr_el2, x1
	msr par_el1, x2
	ldp x1, x2, [x0], #16
	msr sp_el0, x1
	msr sp_el1, x2
	ldp x1, x2, [x0], #16
	msr spsr_el1, x1
	msr tpidr_el0, x2
	ldp x1, x2, [x0], #16
	msr tpidr_el1, x1
	msr tpidrro_el0, x2
	ldp x1, x2, [x0], #16
	msr vbar_el1, x1

	dsb ish
	isb

	ret
