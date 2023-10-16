#ifndef __VM_H__
#define __VM_H__

#include <stdint.h>
#include "config.h"
#include "mm/paging.h"
#include "core/vcpu.h"
#include "core/console.h"

#define VM_WAITING 0
#define VM_RUNNING 1
#define VM_DEAD	   2

struct vm* vm_init(char* name, uint64_t sp, uint64_t entry, uint64_t base);
void prepare_vm();
struct pt_regs* get_vm_pt_regs(struct vm* _vm);
char* vm_state_to_string(uint8_t state);

struct vm_info{
	uint32_t quanta_remaining;
	uint32_t prio;
};

struct pt_regs{
	uint64_t regs[31];
	uint64_t sp;
	uint64_t pc;
	uint64_t pstate;
};

struct vm{
	char name[32];
	uint8_t vmid;
	uint8_t state;
	struct vaddr_space* virtual_address_space;
	struct vcpu cpu;
	struct vm_console input_console;
	struct vm_console output_console;
	struct vm_info info;

	uint64_t* vmdata;
	uint64_t vmdata_size;

	uint64_t load_addr; // in virtual memory
	uint64_t entry; // this is where vm will start executing
	uint64_t sp_init; // this is the initial stack pointer of the vm.
};

#endif
