#include "sse/sse.h"
#include "core/vm.h"
#include "mm/paging.h"
#include "mm/mm.h"
#include "fs/ff.h"
#include "config.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "memory.h"

/*

+   So, basically when the guestOS calls the hypervisor, some args are placed in the general purpose registers. (x0-x7)

+   But when the context switches, these registers are put into VM structure's pt_regs.

+   So now, we need to access those args from pt_regs.

+   Also, the pointers passed from guest are in its IPA. Hence we need to find the page in physical memory as well.

+   Return value from the hypervisor is placed in 'x0' in pt_regs, so when context switch happens back to guest, it can view the return value as though nothing happened.

*/

extern struct vm* current;
extern struct vm* vmlist[CONFIG_MAX_VMs];
extern int total_vms;

FIL* sse_files[CONFIG_MAX_SSE_FILES];

static char* get_sse_vm_dir_name(uint8_t vmid){
    char* name = (char*) malloc(6); // 6 because VMID is <= 255. So VM_255 is the largest string.
    // All this drama is unnecessary. Even though I am asking for 6 bytes, malloc will return an entire page ;). (4096bytes).

    strcpy(name, "VM_");
    strcat(name, itoa(vmid));

    return name;
}

static char* get_sse_vm_dir_path(uint8_t vmid){
    char* name = (char*) malloc(6); 

    strcpy(name, "/VM_");
    strcat(name, itoa(vmid));

    return name;
}



static int sse_sandbox_init(struct vm* vm){

    if(f_chdir("/") != FR_OK) return -1;
    if(f_mkdir(get_sse_vm_dir_name(vm->vmid)) != FR_OK) return -1;    // this will create a directory with VM_{VMID}.
                                                // example: 'VM_1' for VM with id '1'.

    return 0;
}

static int sse_switch_to_vm_sandbox(struct vm* vm){

    if(f_chdir(get_sse_vm_dir_path(vm->vmid)) != FR_OK) return -1;

    return 0;
}

void sse_init(){

    for(int file=0; file < CONFIG_MAX_SSE_FILES; file++){
        sse_files[file] = 0;
    }

    for(int i=0;i<total_vms;i++){
        if(vmlist[i]->sse_enabled){
            if(sse_sandbox_init(vmlist[i]) == 0) printf("LOG: SSE Sandbox initialised for VM: %s\n", vmlist[i]->name);
            else printf("LOG: SSE Sandbox initialization FAILED for VM: %s\n", vmlist[i]->name);
        }
    }


    log("Secure Storage Enclave (SSE) initialised.");
}

void sse_hvc_main_handler(uint16_t hvc_number){

    struct pt_regs* vm_regs = (struct pt_regs*) get_vm_pt_regs(current);   // This is VM's state before context switch occured.

    uint64_t x0 = vm_regs->regs[0];
    uint64_t x1 = vm_regs->regs[1];
    uint64_t x2 = vm_regs->regs[2];

    // temporary variables used inside switch statement.    
    int return_val = -1;
    char* filename_virt;
    char* filename_phys;
    char* buf_virt;
    char* buf_phys;
    size_t size;

    switch(hvc_number){
		case SSE_FREAD_HVC_NR:

            filename_virt = (char*) x0;
            buf_virt    = (char*)x1;
            size        = (size_t)x2;

            filename_phys = (char*) ipa_to_phys(current, (uint64_t) filename_virt);
            buf_phys    = (char*)ipa_to_phys(current, (uint64_t)buf_virt);

			return_val = sse_hvc_fread_handler(filename_phys,buf_phys, size);
			break;
		case SSE_FWRITE_HVC_NR:

            filename_virt = (char*) x0;
            buf_virt    = (char*)x1;
            size        = (size_t)x2;

            filename_phys = (char*) ipa_to_phys(current, (uint64_t) filename_virt);
            buf_phys    = (char*)ipa_to_phys(current, (uint64_t)buf_virt);

			return_val = sse_hvc_fwrite_handler(filename_phys,buf_phys, size);
			break;
	}

    // now we place 'return_val' into VM's pt_regs so it can see it.
    vm_regs->regs[0] = return_val;

}

int  sse_hvc_fread_handler(char* filename, char* buf, size_t size){
    FIL file;

    sse_switch_to_vm_sandbox(current);

    if(f_open(&file, strcat(get_sse_vm_dir_path(current->vmid), filename), FA_READ) != FR_OK) return -1;
    if(f_read(&file, (void*)buf, size,NULL) != FR_OK) return -1;

    f_close(&file);
    return 0;

}
int  sse_hvc_fwrite_handler(char* filename, char* buf, size_t size){

    FIL file;

    if(sse_switch_to_vm_sandbox(current) < 0) return -1;

    char* eff_file_path = get_sse_vm_dir_path(current->vmid);
    strcat(eff_file_path, filename);

    if(f_open(&file, eff_file_path, FA_WRITE | FA_CREATE_NEW) != FR_OK) return -1;
    if(f_write(&file, buf, size, NULL) != FR_OK) return -1;

    f_close(&file);

    return 0;
}