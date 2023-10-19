/*
 *
 * Copyright (c) 2023 Yuvraj Sakshith <ysakshith@gmail.com>
 *
 */


#include "core/delay.h"

void delay_cycles(int nr_cycles){
	while(nr_cycles--) asm volatile("nop");
}

void delay_micro_sec(int nr){
    register unsigned long f, t, r;
    asm volatile ("mrs %0, cntfrq_el0" : "=r"(f));
    asm volatile ("mrs %0, cntpct_el0" : "=r"(t));
    unsigned long i=((f/1000)*nr)/1000;
    do{asm volatile ("mrs %0, cntpct_el0" : "=r"(r));}while(r-t<i);
}

