#include "core/sched.h"
#include "core/vm.h"
#include "config.h"
#include "memory.h"

struct vm* vmlist[CONFIG_MAX_VMs];

void sched_init(){

    memset(vmlist, 0, sizeof(vmlist));

}