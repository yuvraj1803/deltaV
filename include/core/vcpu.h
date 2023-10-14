#ifndef __VCPU_H__
#define __VCPU_H__

#include <stdint.h>

struct vm;
struct vcpu{

    uint64_t (*read_aux)        (struct vm*, uint64_t);
    uint64_t (*read_intctl)     (struct vm*, uint64_t);
    uint64_t (*read_systimer)   (struct vm*, uint64_t);
    uint64_t (*read_gpio)       (struct vm*, uint64_t);
    uint64_t (*read_mmio)       (struct vm*, uint64_t);
    
    void (*write_aux)        (struct vm*, uint64_t,uint64_t);
    void (*write_intctl)     (struct vm*, uint64_t,uint64_t);
    void (*write_systimer)   (struct vm*, uint64_t,uint64_t);
    void (*write_gpio)       (struct vm*, uint64_t,uint64_t);
    void (*write_mmio)       (struct vm*, uint64_t,uint64_t);

	uint8_t (*check_irq_pending)(struct vm*);
	uint8_t (*check_fiq_pending)(struct vm*);


	struct context{
		uint64_t x19;
		uint64_t x20;
		uint64_t x21;
		uint64_t x22;
		uint64_t x23;
		uint64_t x24;
		uint64_t x25;
		uint64_t x26;
		uint64_t x27;
		uint64_t x28;
		uint64_t fp;
		uint64_t sp; 
		uint64_t pc;
	}context;

	struct sysregs{
	    uint64_t sctlr_el1;
		uint64_t ttbr0_el1;
		uint64_t ttbr1_el1;
		uint64_t tcr_el1;
		uint64_t esr_el1;
		uint64_t far_el1;
		uint64_t afsr0_el1;
		uint64_t afsr1_el1;
		uint64_t mair_el1;
		uint64_t amair_el1;
		uint64_t contextidr_el1;
		uint64_t cpacr_el1;
		uint64_t elr_el1;
		uint64_t fpcr;
		uint64_t fpsr;
		uint64_t midr_el1; 
		uint64_t mpidr_el1; 
		uint64_t par_el1;
		uint64_t sp_el0;
		uint64_t sp_el1;
		uint64_t spsr_el1;
		uint64_t tpidr_el0;
		uint64_t tpidr_el1;
		uint64_t tpidrro_el0;
		uint64_t vbar_el1;
		uint64_t actlr_el1; 
		uint64_t id_pfr0_el1; 
		uint64_t id_pfr1_el1; 
		uint64_t id_mmfr0_el1; 
		uint64_t id_mmfr1_el1; 
		uint64_t id_mmfr2_el1; 
		uint64_t id_mmfr3_el1; 
		uint64_t id_isar0_el1; 
		uint64_t id_isar1_el1; 
		uint64_t id_isar2_el1; 
		uint64_t id_isar3_el1; 
		uint64_t id_isar4_el1; 
		uint64_t id_isar5_el1; 
		uint64_t mvfr0_el1; 
		uint64_t mvfr1_el1; 
		uint64_t mvfr2_el1; 
		uint64_t id_aa64pfr0_el1; 
		uint64_t id_aa64pfr1_el1; 
		uint64_t id_aa64dfr0_el1; 
		uint64_t id_aa64dfr1_el1; 
		uint64_t id_aa64isar0_el1; 
		uint64_t id_aa64isar1_el1; 
		uint64_t id_aa64mmfr0_el1; 
		uint64_t id_aa64mmfr1_el1; 
		uint64_t id_aa64afr0_el1; 
		uint64_t id_aa64afr1_el1; 
		uint64_t ctr_el0; 
		uint64_t ccsidr_el1; 
		uint64_t clidr_el1; 
		uint64_t csselr_el1;
		uint64_t aidr_el1; 
		uint64_t revidr_el1; 
		uint64_t cntkctl_el1;
		uint64_t cntp_ctl_el0;
		uint64_t cntp_cval_el0;
		uint64_t cntp_tval_el0;
		uint64_t cntv_ctl_el0;
		uint64_t cntv_cval_el0;
		uint64_t cntv_tval_el0;
	} sysregs;

	struct interrupt_regs{
		uint32_t irq_basic_pending;
		uint32_t irq_pending_1;
		uint32_t irq_pending_2;
		uint32_t fiq_control;
		uint32_t enable_irqs_1;
		uint32_t enable_irqs_2;
		uint32_t enable_basic_irqs;
		uint32_t disable_irqs_1;
		uint32_t disable_irqs_2;
		uint32_t disable_basic_irqs;

		uint32_t enabled_irqs_1;
		uint32_t enabled_irqs_2;
		uint32_t enabled_basic_irqs;

	}interrupt_regs;

	struct aux_regs{
		uint32_t aux_irq;
		uint32_t aux_enables;
		uint32_t aux_mu_io_reg;
		uint32_t aux_mu_ier_reg;
		uint32_t aux_mu_iir_reg;
		uint32_t aux_mu_lcr_reg;
		uint32_t aux_mu_mcr_reg;
		uint32_t aux_mu_lsr_reg;
		uint32_t aux_mu_scratch;
		uint32_t aux_mu_cntl_reg;
		uint32_t aux_mu_stat_reg;
		uint32_t aux_mu_baud_reg;
	} aux_regs;

	struct system_timer_regs{
		uint64_t last_recorded_physical_timer_count; // this will give the timer value when this vcpu was last active.
		uint64_t time_not_active;					 // this will have the time vcpu has spent sitting idle. when other vcpus where being scheduled.

		uint32_t cs;
		uint32_t c0;
		uint32_t c1;
		uint32_t c2;
		uint32_t c3;

	} system_timer_regs;

};


void vcpu_initialise(struct vcpu* cpu);
void vcpu_exit();
void vcpu_enter();

#endif