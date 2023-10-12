#include "core/sync.h"
#include "core/vm.h"
#include "core/sched.h"
#include "mm/paging.h"
#include "mm/mmu.h"
#include "mm/mm.h"
#include "stdio.h"
#include "kstatus.h"


const char *sync_info[] = {
	"Unknown reason.",
	"Trapped WFI or WFE instruction execution.",
	"(unknown)",
	"Trapped MCR or MRC access with (coproc==0b1111).",
	"Trapped MCRR or MRRC access with (coproc==0b1111).",
	"Trapped MCR or MRC access with (coproc==0b1110).",
	"Trapped LDC or STC access.",
	"Access to SVE, Advanced SIMD, or floating-point functionality trapped by CPACR_EL1.FPEN, CPTR_EL2.FPEN, CPTR_EL2.TFP, or CPTR_EL3.TFP control.",
	"Trapped VMRS access, from ID group trap.",
	"Trapped use of a Pointer authentication instruction because HCR_EL2.API == 0 || SCR_EL3.API == 0.",
	"(unknown)",
	"(unknown)",
	"Trapped MRRC access with (coproc==0b1110).",
	"Branch Target Exception.",
	"Illegal Execution state.",
	"(unknown)",
	"(unknown)",
	"SVC instruction execution in AArch32 state.",
	"HVC instruction execution in AArch32 state.",
	"SMC instruction execution in AArch32 state.",
	"(unknown)",
	"SVC instruction execution in AArch64 state.",
	"HVC instruction execution in AArch64 state.",
	"SMC instruction execution in AArch64 state.",
	"Trapped MSR, MRS or System instruction execution in AArch64 state.",
	"Access to SVE functionality trapped as a result of CPACR_EL1.ZEN, CPTR_EL2.ZEN, CPTR_EL2.TZ, or CPTR_EL3.EZ.",
	"Trapped ERET, ERETAA, or ERETAB instruction execution.",
	"(unknown)",
	"Exception from a Pointer Authentication instruction authentication failure.",
	"(unknown)",
	"(unknown)",
	"(unknown)",
	"Instruction Abort from a lower Exception level.",
	"Instruction Abort taken without a change in Exception level.",
	"PC alignment fault exception.",
	"(unknown)",
	"Data Abort from a lower Exception level.",
	"Data Abort without a change in Exception level, or Data Aborts taken to EL2 as a result of accesses generated associated with VNCR_EL2 as part of nested virtualization support.",
	"SP alignment fault exception.",
	"(unknown)",
	"Trapped floating-point exception taken from AArch32 state.",
	"(unknown)",
	"(unknown)",
	"(unknown)",
	"Trapped floating-point exception taken from AArch64 state.",
	"(unknown)",
	"(unknown)",
	"SError interrupt.",
	"Breakpoint exception from a lower Exception level.",
	"Breakpoint exception taken without a change in Exception level.",
	"Software Step exception from a lower Exception level.",
	"Software Step exception taken without a change in Exception level.",
	"Watchpoint from a lower Exception level.",
	"Watchpoint exceptions without a change in Exception level, or Watchpoint exceptions taken to EL2 as a result of accesses generated associated with VNCR_EL2 as part of nested virtualization support.",
	"(unknown)",
	"(unknown)",
	"BKPT instruction execution in AArch32 state.",
	"(unknown)",
	"Vector Catch exception from AArch32 state.",
	"(unknown)",
	"BRK instruction execution in AArch64 state.",
};

#define ESR_EC_SHIFT                          26
#define ESR_EC_WFx_INSTRUCTION                0b000001
#define ESR_EC_FLOATING_POINT_ACCESS          0b000111
#define ESR_EC_HVC_INSTRUCTION                0b010110
#define ESR_EC_SYSREG_ACCESS                  0b011000
#define ESR_EC_DATA_ABORT                     0b100100

#define ESR_DFSC_MASK                         0b111111
#define ESR_SRT_SHIFT                         16
#define ESR_WnR_SHIFT                         6

extern struct vm* current;
extern void halt();

uint8_t get_exception_class(uint64_t esr_el2){
    return (esr_el2 >> ESR_EC_SHIFT) & 0b111111;
}

void handle_sync_wfx(){
    current->cpu.context.pc += 4; // increment PC as we need to acknowledge WFx instruction which caused this trap.
    schedule();
}

void handle_sync_floating_point_access(){
    printf("LOG: Floating point operations not supported.\n");
}

void handle_sync_hvc(uint64_t hvc_number){

}

void handle_sync_sysreg_access(){

}


int8_t handle_sync_data_abort(uint64_t esr_el2, uint64_t far_el2){
    uint8_t dfsc = esr_el2 & ESR_DFSC_MASK; // Data Fault Status Code
    uint8_t WnR =  (esr_el2 >> ESR_WnR_SHIFT)&1;    // Write not Read. Tells data abort caused by reading or writing.
    uint8_t SRT = (esr_el2 >> ESR_SRT_SHIFT) & 0b1111; // Xt/Wt/Rt register involved during the fault.

    if((dfsc >> 2) == 0x1){ // Data fault caused by a translation fault.
        uint64_t phys_to_map = (uint64_t) malloc(PAGE_SIZE);
        uint64_t virt_aborted = far_el2 & ~(PAGE_SIZE-1);

        if(phys_to_map == 0){ // malloc failed to allocate a page.
            return -ERR_MALLOC_FAIL;
        }

        map_page(current, phys_to_map, virt_aborted, MMU_STAGE_2_MEM_FLAGS);
        
    }else{ // Data fault caused by a permission fault. 

    }

    return SUCCESS;

}

void handle_sync(uint64_t esr_el2, uint64_t elr_el2, uint64_t far_el2, uint64_t hvc_number){

    uint8_t EC = get_exception_class(esr_el2);

    switch(EC){

        case ESR_EC_WFx_INSTRUCTION:
            handle_sync_wfx();
            break;

        case ESR_EC_FLOATING_POINT_ACCESS:
            handle_sync_floating_point_access();
            break;

        case ESR_EC_HVC_INSTRUCTION:
            handle_sync_hvc(hvc_number);
            break;
        
        case ESR_EC_SYSREG_ACCESS:
            handle_sync_sysreg_access();
            break;
        
        case ESR_EC_DATA_ABORT:
            int8_t data_abort_handler_status = handle_sync_data_abort(esr_el2, far_el2);
            if(data_abort_handler_status  < 0){
                printf("Data abort could not be handled. ESR: %x FAR: %x\n");
                halt();
            }
            break;

        default:
            printf("LOG: Unsupported sync exception occurred => INFO: %s ESR: %x ELR: %x FAR: %x\n", sync_info[EC], esr_el2, elr_el2, far_el2);
            halt();
            break;
    }

}
