#ifndef __VM_H__
#define __VM_H__

#include <stdint.h>
#include "config.h"
#include "mm/paging.h"

struct vm* new_vm(char* name, uint64_t sp, uint64_t entry, uint64_t base);


struct vm_info{
	uint32_t quanta_remaining;
	uint8_t prio;
};

struct vm_console{
	int in;
	int out;
	char buffer[CONFIG_VM_CONSOLE_BUFFER_SIZE];
};

struct vcpu{
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
};

struct vm{
	char name[32];
	uint8_t vmid;
	uint8_t state;
	struct vaddr_space virtual_address_space;
	struct vcpu cpu;
	struct vm_console console;
	struct vm_info info;

};

#endif