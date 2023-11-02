/*
 *
 * Copyright (c) 2023 Yuvraj Sakshith <ysakshith@gmail.com>
 *
 */


#include "core/vm.h"
#include <stdint.h>
#include "mm/mm.h"
#include "mm/mmu.h"
#include "core/sched.h"
#include "string.h"
#include "config.h"
#include "fs/ff.h"
#include "stdio.h"
#include "kstatus.h"
#include "memory.h"
#include "mm/paging.h"
#include "core/irq.h"

#define PSTATE_INTERRUPT_MASK		(0xf << 6)
#define PSTATE_EL1h					0b0101


struct vm* vmlist[CONFIG_MAX_VMs];
int total_vms = 0;
extern struct vm* current;
extern void __prepare_vm();

extern void assert_virtual_irq();
extern void clear_virtual_irq();
extern void assert_virtual_fiq();
extern void clear_virtual_fiq();


int8_t load_vm(struct vm* _vm, uint64_t sp, uint64_t entry, uint64_t base){
	// make sure base is page aligned.
	base = base & (~PAGE_SIZE); // page aligning just to be sure

	FIL vmfile;
	if(f_open(&vmfile, _vm->name, FA_READ)){
		return -ERR_VM_LOAD_FAIL;
	}

	FILINFO fno;
	f_stat(_vm->name, &fno);
	_vm->vmdata = malloc(fno.fsize);
	_vm->vmdata_size = fno.fsize;

	f_read(&vmfile, _vm->vmdata, fno.fsize, 0);
	f_close(&vmfile);

	_vm->load_addr = base;
	_vm->sp_init = sp;
	_vm->entry = entry;

	if(map_virtual_address_space(_vm) < 0){
		free(_vm->vmdata);
		return -ERR_VM_LOAD_FAIL;
	}



	return SUCCESS;

}

struct pt_regs* get_vm_pt_regs(struct vm* _vm){
	return (struct pt_regs*)((uint64_t)_vm + PAGE_SIZE - sizeof(struct pt_regs));
}

void vm_assert_virtual_interrupt(struct vm* _vm){
	if(_vm->cpu.check_irq_pending(_vm)) assert_virtual_irq();
	else clear_virtual_irq();

	if(_vm->cpu.check_fiq_pending(_vm)) assert_virtual_fiq();
	else clear_virtual_fiq();
}

void prepare_vm(){
	struct pt_regs* regs = get_vm_pt_regs(current);
	regs->pstate = (PSTATE_INTERRUPT_MASK | PSTATE_EL1h);
	regs->sp = current->sp_init;
	regs->pc = current->entry;	
	memset(regs->regs, 0, sizeof(regs->regs));

	load_vttbr_el2(current->vmid, (uint64_t)current->virtual_address_space->lv1_table);
	put_sysregs(&current->cpu.sysregs);
	vm_assert_virtual_interrupt(current);	// initialise virtual interrupts
	
}

struct vm* vm_init(char* name, uint64_t sp, uint64_t entry, uint64_t base){

	if(total_vms == CONFIG_MAX_VMs){
		printf("[LOG]: Failed to load %s. Max VM limit reached.", name);
		return 0;

	}

	struct vm* _vm = malloc(sizeof(struct vm));
	if(!_vm){		
		printf("[LOG]: Failed to load %s.\n", _vm->name);
		return 0;
	}

	strcpy(_vm->name, name);

	_vm->vmid = total_vms;

	_vm->state = VM_RUNNING; // initially vm is in waiting state. until scheduled.

	_vm->cpu.context.pc = (uint64_t) __prepare_vm;
	_vm->cpu.context.sp = (uint64_t) get_vm_pt_regs(_vm);

	get_sysregs(&_vm->cpu.sysregs);
	_vm->cpu.sysregs.sctlr_el1 &= ~1;	// disable MMU.

	_vm->input_console.begin = 0;
	_vm->input_console.end = 0;
	_vm->input_console.used = 0;
	memset(_vm->input_console.buffer, 0, sizeof(_vm->input_console.buffer));

	_vm->output_console.begin = 0;
	_vm->output_console.end = 0;
	_vm->output_console.used = 0;
	memset(_vm->output_console.buffer, 0, sizeof(_vm->output_console.buffer));
	
	_vm->virtual_address_space = create_virtual_address_space(MMU_STAGE_2_MEM_FLAGS, MMU_STAGE_2_MMIO_FLAGS); // we will initialise this later when loading the vm into virtual memory.

	vcpu_initialise(&_vm->cpu);

	_vm->info.prio = CONFIG_SCHED_QUANTA; // all vms have the same priority
	_vm->info.quanta_remaining = CONFIG_SCHED_QUANTA;

	if(load_vm(_vm, sp, entry, base) < 0){
		printf("LOG: Failed to load %s.\n", _vm->name);
		free(_vm);
		return 0;

	}

	vmlist[total_vms++] = _vm;

	printf("LOG: [VMID: %d] %s loaded.\n",_vm->vmid, _vm->name);
	return _vm;
	
}

char* vm_state_to_string(uint8_t state){
	switch(state){
		case 0:
			return "WAITING";
		case 1:
			return "RUNNING";
		case 2:
			return "DEAD";
		default:
			return "Unidentified State";
	}
	return (char*)"Unidentified State";
}