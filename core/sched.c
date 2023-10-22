/*
 *
 * Copyright (c) 2023 Yuvraj Sakshith <ysakshith@gmail.com>
 *
 */


#include "core/sched.h"
#include "core/vm.h"
#include "core/vcpu.h"
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
struct vm* current;

static uint8_t no_vms_scheduled_yet;

void sched_init(){

	struct vm* shell_vm = (struct vm*) malloc(sizeof(struct vm));
	memset(shell_vm, 0, sizeof(struct vm));

	strcpy(shell_vm->name, "Shell");
	shell_vm->state = VM_RUNNING; // shell is initially running.
	shell_vm->vmid = VMID_SHELL;
	shell_vm->cpu.context.pc = (uint64_t) shell_run;
	shell_vm->cpu.context.sp = (uint64_t) get_vm_pt_regs(shell_vm);
	total_vms++;
	vmlist[VMID_SHELL] = shell_vm;
	current = vmlist[VMID_SHELL];

	no_vms_scheduled_yet = 1;

	log("Scheduler initialised");
}

void scheduler_tick(){
	
	current->info.quanta_remaining--;

	if(current->info.quanta_remaining <= 0){
		current->info.quanta_remaining = 0;
		schedule();
	}

}

void schedule(){

	if(no_vms_scheduled_yet){
		no_vms_scheduled_yet = 0;
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
		if(vmlist[i]->state == VM_WAITING && vmlist[i]->info.quanta_remaining > max_quanta){
			max_quanta = vmlist[i]->info.quanta_remaining;
			nextvm = i;
		}
	}

	if(nextvm == -1){ // all vms have no quanta remaining.
		for(int i=0;i<total_vms;i++){
			vmlist[i]->info.quanta_remaining = vmlist[i]->info.prio;
		}
		nextvm = 0;
	}
	struct vm* prev = current;
	current = vmlist[nextvm];

	if(prev == current){	// this will happen when there is only one VM running.
		return;
	}
	
	// update vm states.
	prev->state = VM_WAITING;
	current->state = VM_RUNNING;

	debug("Context switch: %s  -->  %s\n", prev->name, current->name);

	switch_context(&prev->cpu.context, &current->cpu.context);
}
