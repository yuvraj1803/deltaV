#include "drivers/timer.h"
#include "mm/mm.h"
#include "config.h"
#include "stdio.h"

#define TIMER_BASE	(PERIPH_BASE + 0x3000)

#define TIMER_CS	(TIMER_BASE + 0x0)
#define TIMER_CLO	(TIMER_BASE + 0x4)
#define TIMER_CHI	(TIMER_BASE + 0x8)
#define TIMER_C0	(TIMER_BASE + 0xc)
#define TIMER_C1	(TIMER_BASE + 0x10)
#define TIMER_C2	(TIMER_BASE + 0x14
#define TIMER_C3	(TIMER_BASE + 0x18)
#define TIMER_CS_M0	(1U << 0)
#define TIMER_CS_M1	(1U << 1)
#define TIMER_CS_M2	(1U << 2)
#define TIMER_CS_M3	(1U << 3)

const uint32_t timer_interval = CONFIG_SCHED_QUANTA * (1000000/1000); // timer runs at 1MHz
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
}

void system_timer_3_handler(){
	
}
