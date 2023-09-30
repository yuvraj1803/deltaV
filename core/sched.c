#include "core/sched.h"
#include "core/vm.h"
#include "config.h"
#include "memory.h"
#include "stdio.h"
#include "string.h"
#include "mm/paging.h"

extern struct vm* vmlist[CONFIG_MAX_VMs];
extern int total_vms;
struct vm* current;

void sched_init(){

	struct vm* shell_vm = (struct vm*) malloc(sizeof(struct vm));
	memset(shell_vm, 0, sizeof(struct vm));

	strcpy(shell_vm->name, "Shell");
	shell_vm->state = VM_WAITING;
	shell_vm->vmid = 0;
	total_vms++;
	current = vmlist[0];

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
		if(vmlist[i]->info.quanta_remaining > max_quanta){
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
	put_sysregs(&vmlist[nextvm]->cpu.sysregs);
	load_vttbr_el2(vmlist[nextvm]->vmid,vmlist[nextvm]->virtual_address_space->lv1_table);
	switch_context(&current->cpu.context, &vmlist[nextvm]->cpu.context);
}
