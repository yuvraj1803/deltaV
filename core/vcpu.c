/*
 *
 * Copyright (c) 2023 Yuvraj Sakshith <ysakshith@gmail.com>
 *
 */


#include "core/vcpu.h"
#include "memory.h"
#include "mm/mm.h"
#include "core/vm.h"
#include "drivers/timer.h"
#include "stdio.h"
#include "shell/shell.h"
#include "core/console.h"
#include "debug/debug.h"

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

extern struct vm* vmlist[CONFIG_MAX_VMs];
extern struct vm* current;

static uint64_t read_aux_mu(struct vm* _vm, uint64_t addr){
    if((addr >= AUX_MU_IO_REG && addr <= AUX_MU_BAUD_REG) && ((_vm->cpu.aux_regs.aux_enables & 1) == 0)) return 0;  // Invalid AUX MU address being read or AUX MU not enabled.

    switch(addr){
        
        case AUX_MU_IO_REG:
            if(_vm->cpu.aux_regs.aux_mu_lcr_reg & AUX_MU_LCR_DLAB){
                return _vm->cpu.aux_regs.aux_mu_baud_reg & 0xff;
            }else{
                return console_pop(&_vm->output_console);
            }
        case AUX_MU_IER_REG:
            if(_vm->cpu.aux_regs.aux_mu_lcr_reg & AUX_MU_LCR_DLAB){
                return _vm->cpu.aux_regs.aux_mu_baud_reg >> 8;
            }else{
                return _vm->cpu.aux_regs.aux_mu_ier_reg;
            }
        case AUX_MU_IIR_REG:
            int transmit_holding_register_empty = console_empty(&_vm->output_console);
            int reciever_holds_valid_byte = !console_empty(&_vm->input_console);

            int pending = ((transmit_holding_register_empty && (_vm->cpu.aux_regs.aux_mu_ier_reg & 0x2)) | ((reciever_holds_valid_byte && (_vm->cpu.aux_regs.aux_mu_ier_reg & 0x1)) << 1));
            if(pending == 0b11) pending = 0b1;      // bits [1:2] being 0b11 is not possible according to bcm manual.
            return ((pending == 0) | (pending << 1) | (0b11 << 6));
        case AUX_MU_LCR_REG:
            return _vm->cpu.aux_regs.aux_mu_lcr_reg;
        case AUX_MU_MCR_REG:
            return _vm->cpu.aux_regs.aux_mu_mcr_reg;
        case AUX_MU_LSR_REG:
            int data_ready = !console_empty(&_vm->input_console);
            int reciever_overrun = _vm->cpu.aux_regs.reciever_overrun;
            int transmitter_empty = !console_full(&_vm->output_console);
            int transmitter_idle = console_empty(&_vm->output_console);

            _vm->cpu.aux_regs.reciever_overrun = 0; // receiver overrun is always reset when LSR is read.

            return ((data_ready << 0) | (reciever_overrun << 1) | (transmitter_empty << 5) | (transmitter_idle << 6));
        case AUX_MU_MSR_REG:
            return _vm->cpu.aux_regs.aux_mu_msr_reg;
        case AUX_MU_SCRATCH:
            return _vm->cpu.aux_regs.aux_mu_scratch;
        case AUX_MU_CNTL_REG:
            return _vm->cpu.aux_regs.aux_mu_cntl_reg;
        case AUX_MU_STAT_REG:
            int symbol_available = !console_empty(&_vm->input_console);
            int space_available =  !console_full(&_vm->output_console);
            int reciever_idle = console_empty(&_vm->input_console);
                transmitter_idle = !console_empty(&_vm->output_console);
                reciever_overrun = _vm->cpu.aux_regs.reciever_overrun;
            int transmit_fifo_full = !space_available;
            int transmit_fifo_empty = console_empty(&_vm->output_console);
            int transmitter_done = reciever_idle & transmit_fifo_empty;
            int recieve_fifo_fill_level = (_vm->input_console.used > 8 ? 8 : _vm->input_console.used);
            int transmit_fifo_fill_level = (_vm->output_console.used > 8 ? 8 : _vm->output_console.used);;

            return ((symbol_available           << 0) |
                    (space_available            << 1) |
                    (reciever_idle              << 2) |
                    (transmitter_idle           << 3) |
                    (reciever_overrun           << 4) |
                    (transmit_fifo_full         << 5) |
                    (transmit_fifo_empty        << 8) |
                    (transmitter_done           << 9) |
                    (recieve_fifo_fill_level    << 16)|
                    (transmit_fifo_fill_level   << 24));
        case AUX_MU_BAUD_REG:
            return _vm->cpu.aux_regs.aux_mu_baud_reg;

    }

    return 0;
}

uint64_t read_aux(struct vm* _vm, uint64_t addr){
    if(addr >= AUX_MU_IO_REG && addr <= AUX_MU_BAUD_REG) return read_aux_mu(_vm, addr);
    // SPIx support is not added yet

    switch(addr){
        case AUX_IRQ:
            return ~(read_aux_mu(_vm, AUX_MU_IIR_REG) & 1);  // bit 0 of IIR reg is cleared if there is a pending interrupt.
        case AUX_ENABLES:
            return _vm->cpu.aux_regs.aux_enables;
    }


    return 0;

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
        debug("IRQ read at: %x\n", addr);
        return _vm->cpu.read_intctl(_vm,addr);
    }else if(addr >= MMIO_AUX_BEGIN && addr <= MMIO_AUX_END){
        debug("AUX read at: %x\n", addr);
        return _vm->cpu.read_aux(_vm,addr);
    }else if(addr >= MMIO_GPIO_BEGIN && addr <= MMIO_IRQ_END){
        debug("GPIO read at: %x\n", addr);
        return _vm->cpu.read_gpio(_vm,addr);
    }else if(addr >= MMIO_TIMER_BEGIN && addr <= MMIO_TIMER_END){
        debug("TIMER read at: %x\n", addr);
        return _vm->cpu.read_systimer(_vm,addr);
    }

    return 0;
}

static void write_aux_mu(struct vm* _vm, uint64_t addr, uint64_t val){   // mini UART aux write.
    if((addr >= AUX_MU_IO_REG && addr <= AUX_MU_BAUD_REG) && (_vm->cpu.aux_regs.aux_enables & 1) == 0) return;

    switch(addr){
        
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
    if(addr >= AUX_MU_IO_REG && addr <= AUX_MU_BAUD_REG) {
        write_aux_mu(_vm, addr, val);
        return;
    }
    // AUX SPIx modules are not yet supported.

    switch(addr){        
        case AUX_ENABLES:
            _vm->cpu.aux_regs.aux_enables = val;
            break;
    }

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
    current->state = VM_WAITING;
    vmlist[VMID_SHELL]->state = VM_RUNNING;

    // save when vm was last active physically.
    current->cpu.system_timer_regs.last_recorded_physical_timer_count = get_phys_time(); 
}

void vcpu_enter(){
    vmlist[VMID_SHELL]->state = VM_WAITING;
    current->state = VM_RUNNING;

    // once vm is back alive, update for how long it was dead.
    uint64_t current_time = get_phys_time();
    uint64_t time_inactive = current_time - current->cpu.system_timer_regs.last_recorded_physical_timer_count;
    current->cpu.system_timer_regs.time_not_active += time_inactive;
    
    uint64_t current_virtual_time = get_virt_time(current);
    uint32_t min_cx = 0xffffffff;
    uint32_t timers_matched = 0;

    if(current->cpu.system_timer_regs.c0 > current_virtual_time && min_cx > current->cpu.system_timer_regs.c0 - current_virtual_time){
        min_cx = current->cpu.system_timer_regs.c0 - current_virtual_time;
        timers_matched |= TIMER_CS_M0;
    }
    if(current->cpu.system_timer_regs.c1 > current_virtual_time && min_cx > current->cpu.system_timer_regs.c1 - current_virtual_time){
        min_cx = current->cpu.system_timer_regs.c1 - current_virtual_time;
        timers_matched |= TIMER_CS_M1;
    }
    if(current->cpu.system_timer_regs.c2 > current_virtual_time && min_cx > current->cpu.system_timer_regs.c2 - current_virtual_time){
        min_cx = current->cpu.system_timer_regs.c2 - current_virtual_time;
        timers_matched |= TIMER_CS_M2;
    }
    if(current->cpu.system_timer_regs.c3 > current_virtual_time && min_cx > current->cpu.system_timer_regs.c3 - current_virtual_time){
        min_cx = current->cpu.system_timer_regs.c3 - current_virtual_time;
        timers_matched |= TIMER_CS_M3;
    }

    // So what is happening here is, we look at the VM's C0,C1,C2 and C3.
    // Which ever is going to happen immediately next, we store that time in min_cx
    // we configure physical timer C3 for that amount of time.
    // when that occurs, the interrupt will be handled.

    if(min_cx != 0xffffffff) mm_w32(TIMER_C3, TIMER_CLO + min_cx);


    // in this line we check which timers are enabled in the first place and only update TIMER CS for them.
    current->cpu.system_timer_regs.cs |= (~current->cpu.system_timer_regs.cs & timers_matched); 

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
