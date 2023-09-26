#ifndef __SCHED_H__
#define __SCHED_H__

#include "core/vm.h"

void sched_init();
extern void get_sysregs(struct sysregs* _sysregs);
extern void put_sysregs(struct sysregs* _sysregs);
void schedule();
#endif