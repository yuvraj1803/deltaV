/*
 *
 * Copyright (c) 2023 Yuvraj Sakshith <ysakshith@gmail.com>
 *
 */


#include "core/sched.h"
#include "core/vm.h"
#include "core/vcpu.h"
#include "drivers/uart.h"
#include "config.h"
#include "memory.h"
#include "stdio.h"
#include "string.h"
#include "mm/paging.h"
#include "mm/mm.h"
#include "shell/shell.h"
#include "mm/paging.h"
#include "debug/debug.h"

extern struct vm* vmlist[CONFIG_MAX_VMs];
extern int total_vms;
volatile struct vm* current;

void sched_init(){

	struct vm* shell_vm = (struct vm*) malloc(sizeof(struct vm));
	memset(shell_vm, 0, sizeof(struct vm));

	strcpy(shell_vm->name, (const char*)"Shell");
	shell_vm->state = VM_RUNNING; // shell is initially running.
	shell_vm->vmid = VMID_SHELL;

	shell_vm->info.prio = CONFIG_SCHED_QUANTA;
	shell_vm->info.quanta_remaining = CONFIG_SCHED_QUANTA;

	shell_vm->cpu.context.pc = (uint64_t) shell_run;
	shell_vm->cpu.context.sp = (uint64_t) get_vm_pt_regs(shell_vm);

	total_vms++;
	vmlist[VMID_SHELL] = shell_vm;
	current = vmlist[VMID_SHELL];

	log("Scheduler initialised");
}

void scheduler_tick(){
	
	current->info.quanta_remaining--;
	current->info.ticks_passed++;
	
	if(current->info.quanta_remaining <= 0){
		current->info.quanta_remaining = 0;
		schedule();
	}

}

void schedule(){

	if(total_vms == 0){
		struct vm* dummy_vm = (struct vm*) malloc(sizeof(struct vm));
		struct vm* first_vm = vmlist[0];	// shell vm

		// the reason we are creating a dummy vm is to store the current context somewhere in memory.
		// but we wont need this context anytime again so we just put it in some dummy structure and free it.

		switch_context(&dummy_vm->cpu.context, &first_vm->cpu.context);
		return;
	}

	current->info.quanta_remaining = 0;

	int max_quanta = 0;
	int nextvm = -1;

	for(int i=0;i<total_vms;i++){
		if(vmlist[i] != 0 && vmlist[i]->info.quanta_remaining > max_quanta){
			max_quanta = vmlist[i]->info.quanta_remaining;
			nextvm = i;
		}
	}

	if(nextvm == -1){ // all vms have no quanta remaining.
		for(int i=0;i<total_vms;i++){
			vmlist[i]->info.quanta_remaining = (i == vm_connected_to_uart) ? vmlist[i]->info.prio * 3 : vmlist[i]->info.prio;
		}
		nextvm = 0;
	}
	struct vm* prev = (struct vm*) current;
	current = vmlist[nextvm];

	if(prev == current){	// this will happen when there is only one VM running.
		return;
	}
	
	// // update vm states.
	// prev->state = VM_WAITING;
	// current->state = VM_RUNNING;

	debug("Context switch: %s  -->  %s\n", prev->name, current->name);

	load_vttbr_el2(current->vmid, (uint64_t)current->virtual_address_space->lv1_table);
	switch_context(&prev->cpu.context, (struct context*)&current->cpu.context);
}
