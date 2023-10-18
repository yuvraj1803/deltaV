#include "core/sync.h"
#include "core/vm.h"
#include "core/sched.h"
#include "mm/paging.h"
#include "mm/mmu.h"
#include "mm/mm.h"
#include "core/vm.h"
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

#define CHECK_MRS(sysreg_name,_op0,_op1,_CRn,_CRm,_op2)	if(op0 == _op0 && op1 == _op1 && CRn == _CRn && CRm == _CRm && op2 == _op2) \
													regs->regs[rt] = current->cpu.sysregs.sysreg_name;							\
													regs->pc += 4;																\
													return;
													
#define CHECK_MSR(sysreg_name,_op0,_op1,_CRn,_CRm,_op2)	if(op0 == _op0 && op1 == _op1 && CRn == _CRn && CRm == _CRm && op2 == _op2) \
													current->cpu.sysregs.sysreg_name = regs->regs[rt];							\
													regs->pc += 4;																\
													return;
extern struct vm* current;
extern void halt();

uint8_t get_exception_class(uint64_t esr_el2){
    return (esr_el2 >> ESR_EC_SHIFT) & 0b111111;
}

void handle_sync_wfx(){
    get_vm_pt_regs(current)->pc += 4; // increment PC as we need to acknowledge WFx instruction which caused this trap.
    schedule();
}

void handle_sync_floating_point_access(){
    printf("LOG: Floating point operations not supported.\n");
}

void handle_sync_hvc(uint64_t hvc_number){
    // hvc support will be added later
}

static void handle_sync_sysreg_read(uint64_t esr_el2){
	struct pt_regs* regs = get_vm_pt_regs(current);

	uint8_t op0 = (esr_el2 >> 20) & 0b11;
	uint8_t op1 = (esr_el2 >> 14) & 0b111;
	uint8_t op2 = (esr_el2 >> 17) & 0b111;
	uint8_t CRn = (esr_el2 >> 10) & 0b1111;
	uint8_t CRm = (esr_el2 >> 1) & 0b1111;
	uint8_t rt = (esr_el2 >> 5) & 0b11111;

	CHECK_MRS(sctlr_el1,		0b11,0b000,0b0001,0b0000,0b000);
	CHECK_MRS(ttbr0_el1,		0b11,0b000,0b0010,0b0000,0b000);
	CHECK_MRS(ttbr1_el1,		0b11,0b000,0b0010,0b0000,0b001);
	CHECK_MRS(tcr_el1,			0b11,0b000,0b0010,0b0000,0b010);
	CHECK_MRS(esr_el1,			0b11,0b000,0b0101,0b0010,0b000);
	CHECK_MRS(far_el1,			0b11,0b000,0b0110,0b0000,0b000);
	CHECK_MRS(afsr0_el1,		0b11,0b000,0b0101,0b0001,0b000);
	CHECK_MRS(afsr1_el1,		0b11,0b000,0b0101,0b0001,0b001);
	CHECK_MRS(mair_el1,			0b11,0b000,0b1010,0b0010,0b000);
	CHECK_MRS(amair_el1,		0b11,0b000,0b1010,0b0011,0b000);
	CHECK_MRS(contextidr_el1,	0b11,0b000,0b1101,0b0000,0b001);
	CHECK_MRS(cpacr_el1,		0b11,0b000,0b0001,0b0000,0b010);
	CHECK_MRS(elr_el1,			0b11,0b000,0b0100,0b0000,0b001);
	CHECK_MRS(fpcr,				0b11,0b011,0b0100,0b0100,0b000);
	CHECK_MRS(fpsr,				0b11,0b011,0b0100,0b0100,0b001);
	CHECK_MRS(midr_el1,			0b11,0b000,0b0000,0b0000,0b000);
	CHECK_MRS(mpidr_el1,		0b11,0b000,0b0000,0b0000,0b101);
	CHECK_MRS(par_el1,			0b11,0b000,0b0111,0b0100,0b000);
	CHECK_MRS(sp_el0,			0b11,0b000,0b0100,0b0001,0b000);
	CHECK_MRS(sp_el1,			0b11,0b100,0b0100,0b0001,0b000);
	CHECK_MRS(spsr_el1,			0b11,0b000,0b0100,0b0000,0b000);
	CHECK_MRS(tpidr_el0,		0b11,0b011,0b1101,0b0000,0b010);
	CHECK_MRS(tpidr_el1,		0b11,0b000,0b1101,0b0000,0b100);
	CHECK_MRS(tpidrro_el0,		0b11,0b011,0b1101,0b0000,0b011);
	CHECK_MRS(vbar_el1,			0b11,0b000,0b1100,0b0000,0b000);
	CHECK_MRS(actlr_el1,		0b11,0b000,0b0001,0b0000,0b001);
	CHECK_MRS(id_pfr0_el1,		0b11,0b000,0b0000,0b0001,0b000);	
	CHECK_MRS(id_pfr1_el1,		0b11,0b000,0b0000,0b0001,0b001);	
	CHECK_MRS(id_mmfr0_el1,		0b11,0b000,0b0000,0b0001,0b100);
	CHECK_MRS(id_mmfr1_el1,		0b11,0b000,0b0000,0b0001,0b101);
	CHECK_MRS(id_mmfr2_el1,		0b11,0b000,0b0000,0b0001,0b110);
	CHECK_MRS(id_mmfr3_el1,		0b11,0b000,0b0000,0b0001,0b111);
	CHECK_MRS(id_isar0_el1,		0b11,0b000,0b0000,0b0010,0b000);
	CHECK_MRS(id_isar1_el1,		0b11,0b000,0b0000,0b0010,0b001);
	CHECK_MRS(id_isar2_el1,		0b11,0b000,0b0000,0b0010,0b010);
	CHECK_MRS(id_isar3_el1,		0b11,0b000,0b0000,0b0010,0b011);
	CHECK_MRS(id_isar4_el1,		0b11,0b000,0b0000,0b0010,0b100);
	CHECK_MRS(id_isar5_el1,		0b11,0b000,0b0000,0b0010,0b101);
	CHECK_MRS(mvfr0_el1,		0b11,0b000,0b0000,0b0011,0b000);
	CHECK_MRS(mvfr1_el1,		0b11,0b000,0b0000,0b0011,0b001);
	CHECK_MRS(mvfr2_el1,		0b11,0b000,0b0000,0b0011,0b010);
	CHECK_MRS(id_aa64pfr0_el1,	0b11,0b000,0b0000,0b0100,0b000);
	CHECK_MRS(id_aa64pfr1_el1,	0b11,0b000,0b0000,0b0100,0b001);
	CHECK_MRS(id_aa64dfr0_el1,	0b11,0b000,0b0000,0b0101,0b000);
	CHECK_MRS(id_aa64dfr1_el1,	0b11,0b000,0b0000,0b0101,0b001);
	CHECK_MRS(id_aa64isar0_el1,	0b11,0b000,0b0000,0b0110,0b000);
	CHECK_MRS(id_aa64isar1_el1,	0b11,0b000,0b0000,0b0110,0b001);
	CHECK_MRS(id_aa64mmfr0_el1,	0b11,0b000,0b0000,0b0111,0b000);
	CHECK_MRS(id_aa64mmfr1_el1,	0b11,0b000,0b0000,0b0111,0b001);
	CHECK_MRS(id_aa64afr0_el1,	0b11,0b000,0b0000,0b0101,0b100);
	CHECK_MRS(id_aa64afr1_el1,	0b11,0b000,0b0000,0b0101,0b101);
	CHECK_MRS(ctr_el0,			0b11,0b011,0b0000,0b0000,0b001);
	CHECK_MRS(ccsidr_el1,		0b11,0b001,0b0000,0b0000,0b000);
	CHECK_MRS(clidr_el1,		0b11,0b001,0b0000,0b0000,0b001);
	CHECK_MRS(csselr_el1,		0b11,0b010,0b0000,0b0000,0b000);
	CHECK_MRS(aidr_el1,			0b11,0b001,0b0000,0b0000,0b111);
	CHECK_MRS(revidr_el1,		0b11,0b000,0b0000,0b0000,0b110);
	CHECK_MRS(cntkctl_el1,		0b11,0b000,0b1110,0b0001,0b000);
	CHECK_MRS(cntp_ctl_el0,		0b11,0b011,0b1110,0b0010,0b001);
	CHECK_MRS(cntp_cval_el0,	0b11,0b011,0b1110,0b0010,0b010);
	CHECK_MRS(cntp_tval_el0,	0b11,0b011,0b1110,0b0010,0b000);
	CHECK_MRS(cntv_ctl_el0,		0b11,0b011,0b1110,0b0011,0b001);
	CHECK_MRS(cntv_cval_el0,	0b11,0b011,0b1110,0b0011,0b010);
	CHECK_MRS(cntv_tval_el0,	0b11,0b011,0b1110,0b0011,0b000);
	
}

static void handle_sync_sysreg_write(uint64_t esr_el2){

	struct pt_regs* regs = get_vm_pt_regs(current);

	uint8_t op0 = (esr_el2 >> 20) & 0b11;
	uint8_t op1 = (esr_el2 >> 14) & 0b111;
	uint8_t op2 = (esr_el2 >> 17) & 0b111;
	uint8_t CRn = (esr_el2 >> 10) & 0b1111;
	uint8_t CRm = (esr_el2 >> 1) & 0b1111;
	uint8_t rt = (esr_el2 >> 5) & 0b11111;

	CHECK_MSR(sctlr_el1,		0b11,0b000,0b0001,0b0000,0b000);
	CHECK_MSR(ttbr0_el1,		0b11,0b000,0b0010,0b0000,0b000);
	CHECK_MSR(ttbr1_el1,		0b11,0b000,0b0010,0b0000,0b001);
	CHECK_MSR(tcr_el1,			0b11,0b000,0b0010,0b0000,0b010);
	CHECK_MSR(esr_el1,			0b11,0b000,0b0101,0b0010,0b000);
	CHECK_MSR(far_el1,			0b11,0b000,0b0110,0b0000,0b000);
	CHECK_MSR(afsr0_el1,		0b11,0b000,0b0101,0b0001,0b000);
	CHECK_MSR(afsr1_el1,		0b11,0b000,0b0101,0b0001,0b001);
	CHECK_MSR(mair_el1,			0b11,0b000,0b1010,0b0010,0b000);
	CHECK_MSR(amair_el1,		0b11,0b000,0b1010,0b0011,0b000);
	CHECK_MSR(contextidr_el1,	0b11,0b000,0b1101,0b0000,0b001);
	CHECK_MSR(cpacr_el1,		0b11,0b000,0b0001,0b0000,0b010);
	CHECK_MSR(elr_el1,			0b11,0b000,0b0100,0b0000,0b001);
	CHECK_MSR(fpcr,				0b11,0b011,0b0100,0b0100,0b000);
	CHECK_MSR(fpsr,				0b11,0b011,0b0100,0b0100,0b001);
	CHECK_MSR(midr_el1,			0b11,0b000,0b0000,0b0000,0b000);
	CHECK_MSR(mpidr_el1,		0b11,0b000,0b0000,0b0000,0b101);
	CHECK_MSR(par_el1,			0b11,0b000,0b0111,0b0100,0b000);
	CHECK_MSR(sp_el0,			0b11,0b000,0b0100,0b0001,0b000);
	CHECK_MSR(sp_el1,			0b11,0b100,0b0100,0b0001,0b000);
	CHECK_MSR(spsr_el1,			0b11,0b000,0b0100,0b0000,0b000);
	CHECK_MSR(tpidr_el0,		0b11,0b011,0b1101,0b0000,0b010);
	CHECK_MSR(tpidr_el1,		0b11,0b000,0b1101,0b0000,0b100);
	CHECK_MSR(tpidrro_el0,		0b11,0b011,0b1101,0b0000,0b011);
	CHECK_MSR(vbar_el1,			0b11,0b000,0b1100,0b0000,0b000);
	CHECK_MSR(actlr_el1,		0b11,0b000,0b0001,0b0000,0b001);
	CHECK_MSR(csselr_el1,		0b11,0b010,0b0000,0b0000,0b000);
	CHECK_MSR(cntkctl_el1,		0b11,0b000,0b1110,0b0001,0b000);
	CHECK_MSR(cntp_ctl_el0,		0b11,0b011,0b1110,0b0010,0b001);
	CHECK_MSR(cntp_cval_el0,	0b11,0b011,0b1110,0b0010,0b010);
	CHECK_MSR(cntp_tval_el0,	0b11,0b011,0b1110,0b0010,0b000);
	CHECK_MSR(cntv_ctl_el0,		0b11,0b011,0b1110,0b0011,0b001);
	CHECK_MSR(cntv_cval_el0,	0b11,0b011,0b1110,0b0011,0b010);
	CHECK_MSR(cntv_tval_el0,	0b11,0b011,0b1110,0b0011,0b000);
}

void handle_sync_sysreg_access(uint64_t esr_el2){

	int dir = (esr_el2 & 1);

	dir == 1 ? handle_sync_sysreg_read(esr_el2) : handle_sync_sysreg_write(esr_el2);
}


int8_t handle_sync_data_abort(uint64_t esr_el2, uint64_t far_el2){
    uint8_t dfsc = esr_el2 & ESR_DFSC_MASK; // Data Fault Status Code

    if((dfsc >> 2) == 0x1){ // Data fault caused by a translation fault.
        uint64_t phys_to_map = (uint64_t) malloc(PAGE_SIZE);
        uint64_t virt_aborted = far_el2 & ~(PAGE_SIZE-1);

        if(phys_to_map == 0){ // malloc failed to allocate a page.
            return -ERR_MALLOC_FAIL;
        }

        map_page(current, phys_to_map, virt_aborted, MMU_STAGE_2_MEM_FLAGS);
        
    }else{ // Data fault caused by a permission fault. 
        uint8_t WnR =  (esr_el2 >> ESR_WnR_SHIFT)&1;    // Write not Read. Tells data abort caused by reading or writing.
        uint8_t SRT = (esr_el2 >> ESR_SRT_SHIFT) & 0b1111; // Xt/Wt/Rt register involved during the fault.  

        struct pt_regs* regs = get_vm_pt_regs(current);

        if(!WnR){
            regs->regs[SRT] = current->cpu.read_mmio(current, far_el2);
        }else{
            current->cpu.write_mmio(current,far_el2, regs->regs[SRT]);
        }

        regs->pc += 4;

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
            handle_sync_sysreg_access(esr_el2);
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
