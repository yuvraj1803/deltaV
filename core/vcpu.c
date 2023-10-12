#include "core/vcpu.h"
#include "memory.h"
#include "mm/mm.h"
#include "core/vm.h"

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
uint64_t read_intctl(uint64_t addr){

}
uint64_t read_systimer(uint64_t addr){

}
uint64_t read_gpio(uint64_t addr){

}
uint64_t read_mmio(uint64_t addr){
    
    if(addr >= MMIO_IRQ_BEGIN && addr <= MMIO_IRQ_END){
        return current->cpu.read_intctl(addr);
    }else if(addr >= MMIO_AUX_BEGIN && addr <= MMIO_AUX_END){
        return current->cpu.read_aux(addr);
    }else if(addr >= MMIO_GPIO_BEGIN && addr <= MMIO_IRQ_END){
        return current->cpu.read_gpio(addr);
    }else if(addr >= MMIO_TIMER_BEGIN && addr <= MMIO_TIMER_END){
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
    
}

void write_gpio(uint64_t addr,uint64_t val){
    
}

void write_mmio(uint64_t addr,uint64_t val){

    if(addr >= MMIO_IRQ_BEGIN && addr <= MMIO_IRQ_END){
         current->cpu.write_intctl(addr, val);
    }else if(addr >= MMIO_AUX_BEGIN && addr <= MMIO_AUX_END){
         current->cpu.write_aux(addr, val);
    }else if(addr >= MMIO_GPIO_BEGIN && addr <= MMIO_IRQ_END){
         current->cpu.write_gpio(addr, val);
    }else if(addr >= MMIO_TIMER_BEGIN && addr <= MMIO_TIMER_END){
         current->cpu.write_systimer(addr, val);
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

}

void vcpu_enter(){

}