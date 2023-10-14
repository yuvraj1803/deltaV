#include "core/vcpu.h"
#include "memory.h"
#include "mm/mm.h"
#include "core/vm.h"
#include "drivers/timer.h"
#include "stdio.h"
#include "core/console.h"

#define MMIO_IRQ_BEGIN      IRQ_BASIC_PENDING
#define MMIO_IRQ_END        DISABLE_IRQs_2
#define MMIO_AUX_BEGIN      AUX_IRQ
#define MMIO_AUX_END        AUX_MU_BAUD_REG
#define MMIO_GPIO_BEGIN     GPFSEL0
#define MMIO_GPIO_END       GPPUDCLK1
#define MMIO_TIMER_BEGIN    TIMER_CS
#define MMIO_TIMER_END      TIMER_C3

#define FIQ_ENABLE			(1U << 7)
#define AUX_MU_LCR_DLAB     (1U << 7)

extern struct vm* current;

uint64_t read_aux(struct vm* _vm, uint64_t addr){

}

// there are common registers to hold all enabled interrupts.
// this is done to eliminate the necessity for enable AND disable irq regs and to make the implementation simpler.
// when interrupts are enabled, bits are set and when disabled its reset in this same register. check enabled_irqs_1,3,basic.
uint64_t read_intctl(struct vm* _vm, uint64_t addr){
    switch (addr){
        case IRQ_BASIC_PENDING:
            return ((read_intctl(_vm,IRQ_PENDING_1) != 0) << 8) | ((read_intctl(_vm,IRQ_PENDING_2) != 0) << 9);
         case IRQ_PENDING_1:
            uint32_t is_timer1_pending = (_vm->cpu.interrupt_regs.enabled_irqs_1 & (1U << 1)) && (_vm->cpu.system_timer_regs.cs & TIMER_CS_M1);
            uint32_t is_timer3_pending = (_vm->cpu.interrupt_regs.enabled_irqs_1 & (1U << 3)) && (_vm->cpu.system_timer_regs.cs & TIMER_CS_M3);
            return (is_timer1_pending << 1) | (is_timer3_pending << 3);
        case IRQ_PENDING_2:
            uint32_t is_uart_pending = (_vm->cpu.interrupt_regs.enabled_irqs_2 & (57-32)) && (read_aux(_vm,AUX_IRQ) & 1);
            return is_uart_pending << (57 - 32);
        case FIQ_CONTROL:
            return _vm->cpu.interrupt_regs.fiq_control;
            break;
        case ENABLE_IRQs_1:
            return _vm->cpu.interrupt_regs.enabled_irqs_1;
            break;
        case ENABLE_IRQs_2:
            return _vm->cpu.interrupt_regs.enabled_irqs_2;
            break;
        case ENABLE_BASIC_IRQs:
            return _vm->cpu.interrupt_regs.enabled_basic_irqs;
            break;
        case DISABLE_IRQs_1:
            return ~_vm->cpu.interrupt_regs.enabled_irqs_1;
            break;
        case DISABLE_IRQs_2:
            return ~_vm->cpu.interrupt_regs.enabled_irqs_2;
            break;
        case DISABLE_BASIC_IRQs:
            return ~_vm->cpu.interrupt_regs.enabled_basic_irqs;
            break;    
    }

    return 0;
}
uint64_t read_systimer(struct vm* _vm, uint64_t addr){
    switch(addr){
        case TIMER_CS:
            return _vm->cpu.system_timer_regs.cs;
        case TIMER_CHI:
            return get_virt_time(_vm) >> 32;
        case TIMER_CLO:
            return get_virt_time(_vm) & 0xffffffff;
        case TIMER_C0:
            return _vm->cpu.system_timer_regs.c0;
        case TIMER_C1:
            return _vm->cpu.system_timer_regs.c1;
        case TIMER_C2:
            return _vm->cpu.system_timer_regs.c2;
        case TIMER_C3:
            return _vm->cpu.system_timer_regs.c3;
    }

    return 0;
}
uint64_t read_gpio(struct vm* _vm, uint64_t addr){
    // gpio read functionality will be added later.
    printf("GPIO read at: %x\n", addr);

    return 0;
}
uint64_t read_mmio(struct vm* _vm, uint64_t addr){
    
    if(addr >= MMIO_IRQ_BEGIN && addr <= MMIO_IRQ_END){
        // printf("IRQ read at: %x\n", addr);
        return _vm->cpu.read_intctl(_vm,addr);
    }else if(addr >= MMIO_AUX_BEGIN && addr <= MMIO_AUX_END){
        // printf("AUX read at: %x\n", addr);
        return _vm->cpu.read_aux(_vm,addr);
    }else if(addr >= MMIO_GPIO_BEGIN && addr <= MMIO_IRQ_END){
        // printf("GPIO read at: %x\n", addr);
        return _vm->cpu.read_gpio(_vm,addr);
    }else if(addr >= MMIO_TIMER_BEGIN && addr <= MMIO_TIMER_END){
        // printf("TIMER read at: %x\n", addr);
        return _vm->cpu.read_systimer(_vm,addr);
    }

    return 0;
}

static void write_aux_mu(struct vm* _vm, uint64_t addr, uint64_t val){   // mini UART aux write.
    if((addr >= AUX_MU_IO_REG && addr <= AUX_MU_BAUD_REG && (_vm->cpu.aux_regs.aux_enables & 1)) == 0) return;

    switch(addr){
        case AUX_ENABLES:
            _vm->cpu.aux_regs.aux_enables = val;
            break;
        
        // for AUX_MU_IO_REG and AUX_MU_IER_REG, if the DLAB bit is set in the AUX_MU_LCR_REG, the access is given to the AUX_MU_BAUD_REG
        // AUX_MU_IO_REG gives us access to the least significant 8 bits in LCR
        // AUX_MU_IER_REG gives us access to the most significant 8 bits in the LCR (out of 16 bits).
        // Hence we need to check this explicitly.
        // For more information read the Broadcom BCM2835 Manual.
        case AUX_MU_IO_REG:
            if(_vm->cpu.aux_regs.aux_mu_lcr_reg & AUX_MU_LCR_DLAB){
                _vm->cpu.aux_regs.aux_mu_lcr_reg &= ~AUX_MU_LCR_DLAB;
                _vm->cpu.aux_regs.aux_mu_baud_reg = (_vm->cpu.aux_regs.aux_mu_baud_reg & 0xff) | (val & 0xff);
            }else{
                console_push(&_vm->output_console, val);    // push the given value to the vm's output console.
            }
            break;
        case AUX_MU_IER_REG:
             if(_vm->cpu.aux_regs.aux_mu_lcr_reg & AUX_MU_LCR_DLAB){
                _vm->cpu.aux_regs.aux_mu_baud_reg = (_vm->cpu.aux_regs.aux_mu_baud_reg & 0xff00) | (val & 0xff);
            }else{
                _vm->cpu.aux_regs.aux_mu_ier_reg = val;
            }
            break;
        case AUX_MU_IIR_REG:
            if(val & 0x2){
                console_clear(&_vm->input_console);  // clear the input console for the vm.
            }else{
                console_clear(&_vm->output_console); // clear the output console for the vm.
            }
            break;
        case AUX_MU_LCR_REG:
            _vm->cpu.aux_regs.aux_mu_lcr_reg = val;
            break;
        case AUX_MU_MCR_REG:
            _vm->cpu.aux_regs.aux_mu_mcr_reg = val;
            break; 
        case AUX_MU_SCRATCH:
            _vm->cpu.aux_regs.aux_mu_scratch = val;
            break;
        case AUX_MU_CNTL_REG:
            _vm->cpu.aux_regs.aux_mu_cntl_reg = val;
            break;
        case AUX_MU_BAUD_REG:
            _vm->cpu.aux_regs.aux_mu_baud_reg = val;
            break;

    }

}

void write_aux(struct vm* _vm, uint64_t addr,uint64_t val){
    write_aux_mu(_vm, addr, val);
    // AUX SPIx modules are not yet supported.
}

void write_intctl(struct vm* _vm, uint64_t addr,uint64_t val){
    
    switch (addr){
        case FIQ_CONTROL:
            _vm->cpu.interrupt_regs.fiq_control = val;
            break;
        case ENABLE_IRQs_1:
            _vm->cpu.interrupt_regs.enable_irqs_1 = 1;
            _vm->cpu.interrupt_regs.enabled_irqs_1 |= val;
            break;
        case ENABLE_IRQs_2:
            _vm->cpu.interrupt_regs.enable_irqs_2 = 1;
            _vm->cpu.interrupt_regs.enabled_irqs_2 |= val;
            break;
        case ENABLE_BASIC_IRQs:
            _vm->cpu.interrupt_regs.enable_basic_irqs = val;
            _vm->cpu.interrupt_regs.enabled_basic_irqs |= val;
            break;
        case DISABLE_IRQs_1:
            _vm->cpu.interrupt_regs.disable_irqs_1 = val;
            _vm->cpu.interrupt_regs.enabled_irqs_1 &= ~val;
            break;
        case DISABLE_IRQs_2:
            _vm->cpu.interrupt_regs.disable_irqs_2 = val;
            _vm->cpu.interrupt_regs.enabled_irqs_2 &= ~val;
            break;
        case DISABLE_BASIC_IRQs:
            _vm->cpu.interrupt_regs.disable_basic_irqs = val;
            _vm->cpu.interrupt_regs.enabled_basic_irqs &= ~val;
            break;
    }

}

void write_systimer(struct vm* _vm, uint64_t addr,uint64_t val){
    switch(addr){
        case TIMER_CS:
            _vm->cpu.system_timer_regs.cs &= ~val;
            break;
        case TIMER_C0:
            _vm->cpu.system_timer_regs.c0 = val;
            break;
        case TIMER_C1:
            _vm->cpu.system_timer_regs.c1 = val;
            break;
        case TIMER_C2:
            _vm->cpu.system_timer_regs.c2 = val;
            break;
        case TIMER_C3:
            _vm->cpu.system_timer_regs.c3 = val;
            break;
    }
}

void write_gpio(struct vm* _vm, uint64_t addr,uint64_t val){
     // gpio read functionality will be added later.
    printf("GPIO write at: %x\n", addr);
}

void write_mmio(struct vm* _vm, uint64_t addr,uint64_t val){

    if(addr >= MMIO_IRQ_BEGIN && addr <= MMIO_IRQ_END){
        _vm->cpu.write_intctl(_vm,addr, val);
        // printf("IRQ write at: %x\n", addr);
    }else if(addr >= MMIO_AUX_BEGIN && addr <= MMIO_AUX_END){
        _vm->cpu.write_aux(_vm,addr, val);
        // printf("AUX write at: %x\n", addr);
    }else if(addr >= MMIO_GPIO_BEGIN && addr <= MMIO_IRQ_END){
        _vm->cpu.write_gpio(_vm,addr, val);
        // printf("GPIO write at: %x\n", addr);
    }else if(addr >= MMIO_TIMER_BEGIN && addr <= MMIO_TIMER_END){
        _vm->cpu.write_systimer(_vm,addr, val);
        // printf("TIMER write at: %x\n", addr);
    }
    
}

void vcpu_exit(){
    // save when vm was last active physically.
    current->cpu.system_timer_regs.last_recorded_physical_timer_count = get_phys_time(); 
}

void vcpu_enter(){
    // once vm is back alive, update for how long it was dead.
    uint64_t current_time = get_phys_time();
    uint64_t time_inactive = current_time - current->cpu.system_timer_regs.last_recorded_physical_timer_count;
    current->cpu.system_timer_regs.time_not_active += time_inactive;
}

uint8_t check_irq_pending(struct vm* _vm){
    return _vm->cpu.read_intctl(_vm,IRQ_BASIC_PENDING) != 0;    // check of an irq is still unserviced in this vcpu.
}

uint8_t check_fiq_pending(struct vm* _vm){

    if((_vm->cpu.interrupt_regs.fiq_control & FIQ_ENABLE) == 0) return 0;   // fiqs are not enabled at all.

    int enabled_fiqs = _vm->cpu.interrupt_regs.fiq_control & 0b1111111;

    if(enabled_fiqs >= 0 && enabled_fiqs <= 31){
        int pending_interrupts = _vm->cpu.read_intctl(_vm,IRQ_PENDING_1);
        return pending_interrupts & (1U << enabled_fiqs);
    }

    if(enabled_fiqs >= 32 && enabled_fiqs <= 63){
        int pending_interrupts = _vm->cpu.read_intctl(_vm,IRQ_PENDING_2);
        return pending_interrupts & (1U << (enabled_fiqs-32));
    }

    if(enabled_fiqs >= 64 && enabled_fiqs <= 71){
        int pending_interrupts = _vm->cpu.read_intctl(_vm,IRQ_BASIC_PENDING);
        return pending_interrupts & (1U << (enabled_fiqs - 64));
    }

    return 0;

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

    cpu->check_irq_pending = check_irq_pending;
    cpu->check_fiq_pending = check_fiq_pending;
}
