#include "core/vcpu.h"
#include "memory.h"
#include "mm/mm.h"
#include "core/vm.h"
#include "drivers/timer.h"

#define MMIO_IRQ_BEGIN      IRQ_BASIC_PENDING
#define MMIO_IRQ_END        DISABLE_IRQs_2
#define MMIO_AUX_BEGIN      AUX_IRQ
#define MMIO_AUX_END        AUX_MU_BAUD_REG
#define MMIO_GPIO_BEGIN     GPFSEL0
#define MMIO_GPIO_END       GPPUDCLK1
#define MMIO_TIMER_BEGIN    TIMER_CS
#define MMIO_TIMER_END      TIMER_C3

extern struct vm* current;

uint64_t read_aux(uint64_t addr){

}

// there are common registers to hold all enabled interrupts.
// this is done to eliminate the necessity for enable AND disable irq regs and to make the implementation simpler.
// when interrupts are enabled, bits are set and when disabled its reset in this same register. check enabled_irqs_1,3,basic.
uint64_t read_intctl(uint64_t addr){
    switch (addr){
        case IRQ_BASIC_PENDING:
            return ((read_intctl(IRQ_PENDING_1) != 0) << 8) | ((read_intctl(IRQ_PENDING_2) != 0) << 9);
         case IRQ_PENDING_1:
            uint32_t is_timer1_pending = (current->cpu.interrupt_regs.enabled_irqs_1 & (1U << 1)) && (current->cpu.system_timer_regs.cs & TIMER_CS_M1);
            uint32_t is_timer3_pending = (current->cpu.interrupt_regs.enabled_irqs_1 & (1U << 3)) && (current->cpu.system_timer_regs.cs & TIMER_CS_M3);
            return (is_timer1_pending << 1) | (is_timer3_pending << 3);
        case IRQ_PENDING_2:
            uint32_t is_uart_pending = (current->cpu.interrupt_regs.enabled_irqs_2 & (57-32)) && (read_aux(AUX_IRQ) & 1);
            return is_uart_pending << (57 - 32);
        case FIQ_CONTROL:
            return current->cpu.interrupt_regs.fiq_control;
            break;
        case ENABLE_IRQs_1:
            return current->cpu.interrupt_regs.enabled_irqs_1;
            break;
        case ENABLE_IRQs_2:
            return current->cpu.interrupt_regs.enabled_irqs_2;
            break;
        case ENABLE_BASIC_IRQs:
            return current->cpu.interrupt_regs.enabled_basic_irqs;
            break;
        case DISABLE_IRQs_1:
            return ~current->cpu.interrupt_regs.enabled_irqs_1;
            break;
        case DISABLE_IRQs_2:
            return ~current->cpu.interrupt_regs.enabled_irqs_2;
            break;
        case DISABLE_BASIC_IRQs:
            return ~current->cpu.interrupt_regs.enabled_basic_irqs;
            break;    
    }
}
uint64_t read_systimer(uint64_t addr){
    switch(addr){
        case TIMER_CS:
            return current->cpu.system_timer_regs.cs;
        case TIMER_CHI:
            return get_virt_time(current) >> 32;
        case TIMER_CLO:
            return get_virt_time(current) & 0xffffffff;
        case TIMER_C0:
            return current->cpu.system_timer_regs.c0;
        case TIMER_C1:
            return current->cpu.system_timer_regs.c1;
        case TIMER_C2:
            return current->cpu.system_timer_regs.c2;
        case TIMER_C3:
            return current->cpu.system_timer_regs.c3;
    }

    return 0;
}
uint64_t read_gpio(uint64_t addr){
    // gpio read functionality will be added later.
    printf("GPIO read at: %x\n", addr);
}
uint64_t read_mmio(uint64_t addr){
    
    if(addr >= MMIO_IRQ_BEGIN && addr <= MMIO_IRQ_END){
        // printf("IRQ read at: %x\n", addr);
        return current->cpu.read_intctl(addr);
    }else if(addr >= MMIO_AUX_BEGIN && addr <= MMIO_AUX_END){
        // printf("AUX read at: %x\n", addr);
        return current->cpu.read_aux(addr);
    }else if(addr >= MMIO_GPIO_BEGIN && addr <= MMIO_IRQ_END){
        // printf("GPIO read at: %x\n", addr);
        return current->cpu.read_gpio(addr);
    }else if(addr >= MMIO_TIMER_BEGIN && addr <= MMIO_TIMER_END){
        // printf("TIMER read at: %x\n", addr);
        return current->cpu.read_systimer(addr);
    }

    return 0;
}
    
void write_aux(uint64_t addr,uint64_t val){

}

void write_intctl(uint64_t addr,uint64_t val){
    
    switch (addr){
        case FIQ_CONTROL:
            current->cpu.interrupt_regs.fiq_control = val;
            break;
        case ENABLE_IRQs_1:
            current->cpu.interrupt_regs.enable_irqs_1 = 1;
            current->cpu.interrupt_regs.enabled_irqs_1 |= val;
            break;
        case ENABLE_IRQs_2:
            current->cpu.interrupt_regs.enable_irqs_2 = 1;
            current->cpu.interrupt_regs.enabled_irqs_2 |= val;
            break;
        case ENABLE_BASIC_IRQs:
            current->cpu.interrupt_regs.enable_basic_irqs = val;
            current->cpu.interrupt_regs.enabled_basic_irqs |= val;
            break;
        case DISABLE_IRQs_1:
            current->cpu.interrupt_regs.disable_irqs_1 = val;
            current->cpu.interrupt_regs.enabled_irqs_1 &= ~val;
            break;
        case DISABLE_IRQs_2:
            current->cpu.interrupt_regs.disable_irqs_2 = val;
            current->cpu.interrupt_regs.enabled_irqs_2 &= ~val;
            break;
        case DISABLE_BASIC_IRQs:
            current->cpu.interrupt_regs.disable_basic_irqs = val;
            current->cpu.interrupt_regs.enabled_basic_irqs &= ~val;
            break;
    }

}

void write_systimer(uint64_t addr,uint64_t val){
    switch(addr){
        case TIMER_CS:
            current->cpu.system_timer_regs.cs &= ~val;
            break;
        case TIMER_C0:
            current->cpu.system_timer_regs.c0 = val;
            break;
        case TIMER_C1:
            current->cpu.system_timer_regs.c1 = val;
            break;
        case TIMER_C2:
            current->cpu.system_timer_regs.c2 = val;
            break;
        case TIMER_C3:
            current->cpu.system_timer_regs.c3 = val;
            break;
    }
}

void write_gpio(uint64_t addr,uint64_t val){
     // gpio read functionality will be added later.
    printf("GPIO write at: %x\n", addr);
}

void write_mmio(uint64_t addr,uint64_t val){

    if(addr >= MMIO_IRQ_BEGIN && addr <= MMIO_IRQ_END){
        current->cpu.write_intctl(addr, val);
        // printf("IRQ write at: %x\n", addr);
    }else if(addr >= MMIO_AUX_BEGIN && addr <= MMIO_AUX_END){
        current->cpu.write_aux(addr, val);
        // printf("AUX write at: %x\n", addr);
    }else if(addr >= MMIO_GPIO_BEGIN && addr <= MMIO_IRQ_END){
        current->cpu.write_gpio(addr, val);
        // printf("GPIO write at: %x\n", addr);
    }else if(addr >= MMIO_TIMER_BEGIN && addr <= MMIO_TIMER_END){
        current->cpu.write_systimer(addr, val);
        // printf("TIMER write at: %x\n", addr);
    }
    
}

void vcpu_initialise(struct vcpu* cpu){
    memset(&cpu->interrupt_regs, 0, sizeof(cpu->interrupt_regs));
	memset(&cpu->system_timer_regs, 0, sizeof(cpu->system_timer_regs));
	memset(&cpu->aux_regs, 0, sizeof(cpu->aux_regs));

    cpu->read_aux       = read_aux;
    cpu->read_intctl    = read_intctl;
    cpu->read_systimer  = read_systimer;
    cpu->read_gpio      = read_gpio;
    cpu->read_mmio      = read_mmio;

    cpu->write_aux      = write_aux;
    cpu->write_intctl   = write_intctl;
    cpu->write_systimer = write_systimer;
    cpu->write_gpio     = write_gpio;
    cpu->write_mmio     = write_mmio;
}

void vcpu_exit(){
    current->cpu.system_timer_regs.last_recorded_physical_timer_count = get_phys_time(); 
}

void vcpu_enter(){
    uint64_t current_time = get_phys_time();
    uint64_t time_inactive = current_time - current->cpu.system_timer_regs.last_recorded_physical_timer_count;
    current->cpu.system_timer_regs.time_not_active += time_inactive;
}