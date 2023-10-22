/*
 *
 * Copyright (c) 2023 Yuvraj Sakshith <ysakshith@gmail.com>
 *
 */


#include "drivers/timer.h"
#include "mm/mm.h"
#include "config.h"
#include "stdio.h"
#include "core/sched.h"
#include "core/vm.h"


const uint32_t timer_interval = (1000000/1000); // timer runs at 1MHz
// (1000000/1000) is number of cycles in 1 ms)

void timer_info(){
	printf("CS: %x\n", mm_r32(TIMER_CS));
	printf("CLO: %x\n", mm_r32(TIMER_CLO));
	printf("CHI: %x\n", mm_r32(TIMER_CHI));
}
void timer_init(void){
	mm_w32(TIMER_C1,mm_r32(TIMER_CLO) + timer_interval);

	log("Timer initialised");
}

void system_timer_1_handler(){

	mm_w32(TIMER_C1,  mm_r32(TIMER_CLO) + timer_interval);
	mm_w32(TIMER_CS, TIMER_CS_M1);
	scheduler_tick();
}

void system_timer_3_handler(){
	mm_w32(TIMER_CS, TIMER_CS_M3); 	// this timer acts as a virtual timer.
									// all 4 timers of a VM are virtualised into a single timer.
									// check vcpu_enter() for its implementation.
}

uint64_t get_phys_time(){
	return mm_r32(TIMER_CLO) | mm_r32(TIMER_CHI);
}

uint64_t get_virt_time(struct vm* _vm){
	return get_phys_time() - _vm->cpu.system_timer_regs.time_not_active;
}

