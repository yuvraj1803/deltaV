#ifndef __TIMER_H__
#define __TIMER_H__

#include "core/vm.h"

void timer_init();
void timer_info();
void system_timer_1_handler();
void system_timer_3_handler();
uint64_t get_virt_time(struct vm* _vm);
uint64_t get_phys_time();

#endif
