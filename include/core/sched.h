#ifndef __SCHED_H__
#define __SCHED_H__

#include "core/vm.h"

void sched_init();
extern void load_sysregs(struct sysregs* _sysregs);
#endif