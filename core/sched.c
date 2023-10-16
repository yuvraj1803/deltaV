#include "core/sched.h"
#include "core/vm.h"
#include "config.h"
#include "memory.h"
#include "stdio.h"
#include "string.h"
#include "mm/paging.h"
#include "mm/mm.h"
#include "shell/shell.h"
#include "mm/paging.h"

extern struct vm* vmlist[CONFIG_MAX_VMs];
extern int total_vms;
struct vm* current;

void sched_init(){

	struct vm* shell_vm = (struct vm*) malloc(sizeof(struct vm));
	memset(shell_vm, 0, sizeof(struct vm));

	strcpy(shell_vm->name, "Shell");
	shell_vm->state = VM_RUNNING; // shell is initially running.
	shell_vm->vmid = VMID_SHELL;
	total_vms++;
	vmlist[VMID_SHELL] = shell_vm;
	current = vmlist[VMID_SHELL];

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
	
	// update vm states.
	prev->state = VM_WAITING;
	current->state = VM_RUNNING;

	switch_context(&prev->cpu.context, &current->cpu.context);
}
