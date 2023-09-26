.globl get_sysregs
.globl put_sysregs

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
	msr midr_el1, x2
	ldp x1, x2, [x0], #16
	msr mpidr_el1, x1
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
	msr actlr_el1, x2
	ldp x1, x2, [x0], #16
	msr id_pfr0_el1, x1
	msr id_pfr1_el1, x2
	ldp x1, x2, [x0], #16
	msr id_mmfr0_el1, x1
	msr id_mmfr1_el1, x2
	ldp x1, x2, [x0], #16
	msr id_mmfr2_el1, x1
	msr id_mmfr3_el1, x2
	ldp x1, x2, [x0], #16
	msr id_isar0_el1, x1
	msr id_isar1_el1, x2
	ldp x1, x2, [x0], #16
	msr id_isar2_el1, x1
	msr id_isar3_el1, x2
	ldp x1, x2, [x0], #16
	msr id_isar4_el1, x1
	msr id_isar5_el1, x2
	ldp x1, x2, [x0], #16
	msr mvfr0_el1, x1
	msr mvfr1_el1, x2
	ldp x1, x2, [x0], #16
	msr mvfr2_el1, x1
	msr id_aa64pfr0_el1, x2
	ldp x1, x2, [x0], #16
	msr id_aa64pfr1_el1, x1
	msr id_aa64dfr0_el1, x2
	ldp x1, x2, [x0], #16
	msr id_aa64dfr1_el1, x1
	msr id_aa64isar0_el1, x2
	ldp x1, x2, [x0], #16
	msr id_aa64isar1_el1, x1
	msr id_aa64mmfr0_el1, x2
	ldp x1, x2, [x0], #16
	msr id_aa64mmfr1_el1, x1
	msr id_aa64afr0_el1, x2
	ldp x1, x2, [x0], #16
	msr id_aa64afr1_el1, x1
	msr ctr_el0, x2
	ldp x1, x2, [x0], #16
	msr ccsidr_el1, x1
	msr clidr_el1, x2
	ldp x1, x2, [x0], #16
	msr csselr_el1, x1
	msr aidr_el1, x2
	ldp x1, x2, [x0], #16
	msr revidr_el1, x1
	msr cntkctl_el1, x2
	ldp x1, x2, [x0], #16
	msr cntp_ctl_el0, x1
	msr cntp_cval_el0, x2
	ldp x1, x2, [x0], #16
	msr cntp_tval_el0, x1
	msr cntv_ctl_el0, x2
	ldp x1, x2, [x0], #16
	msr cntv_cval_el0, x1
	msr cntv_tval_el0, x2
	ret
