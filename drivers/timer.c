#include "drivers/timer.h"
#include "mm/mm.h"

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

const uint32_t timer_interval = 100000;


void timer_init(void){
	mm_w(TIMER_CS,	(mm_r(TIMER_CLO) & 0xffffffff) + timer_interval);
}