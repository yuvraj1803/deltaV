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

struct vm* vmlist[CONFIG_MAX_VMs];
static int total_vms = 0;

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

	_vm->cpu.context.sp = sp;
	_vm->cpu.context.pc = entry;

	_vm->load_addr = base;

	if(map_virtual_address_space(_vm) < 0){
		free(_vm->vmdata);
		return -ERR_VM_LOAD_FAIL;
	}



	return SUCCESS;

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

	_vm->state = VM_WAITING; // initially vm is in waiting state. until scheduled.

	get_sysregs(&_vm->cpu.sysregs);
	_vm->cpu.sysregs.sctlr_el1 &= ~1;	// disable MMU.

	_vm->console.in = 0;
	_vm->console.out = 0;
	memset(_vm->console.buffer, 0, sizeof(_vm->console.buffer));
	
	_vm->virtual_address_space = create_virtual_address_space(MMU_STAGE_2_MEM_FLAGS, MMU_STAGE_2_MMIO_FLAGS); // we will initialise this later when loading the vm into virtual memory.

	memset(&_vm->cpu.interrupt_regs, 0, sizeof(_vm->cpu.interrupt_regs));
	memset(&_vm->cpu.system_timer_regs, 0, sizeof(_vm->cpu.system_timer_regs));
	memset(&_vm->cpu.aux_regs, 0, sizeof(_vm->cpu.aux_regs));

	_vm->info.prio = 0; // all vms have the same priority
	_vm->info.quanta_remaining = CONFIG_SCHED_QUANTA;

	vmlist[total_vms++] = _vm;

	if(load_vm(_vm, sp, entry, base) < 0){
		printf("LOG: Failed to load %s.\n", _vm->name);
		free(_vm);
		return 0;

	}

	printf("LOG: %s loaded.\n", _vm->name);
	return _vm;
	
}
