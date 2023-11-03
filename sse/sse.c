#include "sse/sse.h"
#include "core/vm.h"
#include "mm/paging.h"

/*

+   So, basically when the guestOS calls the hypervisor, some args are placed in the general purpose registers. (x0-x7)

+   But when the context switches, these registers are put into VM structure's pt_regs.

+   So now, we need to access those args from pt_regs.

+   Also, the pointers passed from guest are in its IPA. Hence we need to find the page in physical memory as well.

+   Return value from the hypervisor is placed in 'x0' in pt_regs, so when context switch happens back to guest, it can view the return value as though nothing happened.

*/

extern struct vm* current;

void sse_hvc_main_handler(uint16_t hvc_number){

    struct pt_regs* vm_regs = (struct pt_regs*) get_vm_pt_regs(current);   // This is VM's state before context switch occured.

    uint64_t x0 = vm_regs->regs[0];
    uint64_t x1 = vm_regs->regs[1];
    uint64_t x2 = vm_regs->regs[2];
    uint64_t x3 = vm_regs->regs[3];


    // temporary variables used inside switch statement.    
    int return_val = -1;
    char* filename_virt;
    char* filename_phys;
    char* mode_virt;
    char* mode_phys;
    char* buf_virt;
    char* buf_phys;
    size_t size, nmemb;
    int fd;
    int pos;
    uint64_t offset;

    switch(hvc_number){
		case SSE_FOPEN_HVC_NR:

            filename_virt = (char*)x0;
            mode_virt     = (char*)x1;

            filename_phys = (char*)ipa_to_phys(current, (uint64_t)filename_virt);
            mode_phys     = (char*)ipa_to_phys(current, (uint64_t)mode_virt);

			return_val = sse_hvc_fopen_handler(filename_phys, mode_phys);
			break;
		case SSE_FREAD_HVC_NR:

            buf_virt    = (char*)x0;
            size        = (size_t)x1;
            nmemb       = (size_t)x2;
            fd          = (int)x3;

            buf_phys    = (char*)ipa_to_phys(current, (uint64_t)buf_virt);

			return_val = sse_hvc_fread_handler(buf_phys, size, nmemb, fd);
			break;
		case SSE_FWRITE_HVC_NR:

            buf_virt        = (char*)x0;
            size            = (size_t)x1;
            nmemb           = (size_t)x2;
            fd              = (int)x3;

            buf_phys        = (char*)ipa_to_phys(current, (uint64_t)buf_virt);

			return_val = sse_hvc_fwrite_handler(buf_phys, size, nmemb, fd);
			break;
		case SSE_FSEEK_HVC_NR:
            fd      = (int)x0;
            offset  = (uint64_t)x1;
            pos     = (int)x2;
			return_val = sse_hvc_fseek_handler(fd, offset, pos);
			break;
		case SSE_REWIND_HVC_NR:

            fd = (int)x0;
			return_val = sse_hvc_rewind_handler(fd);
			break;
		case SSE_FCLOSE_HVC_NR:
            fd = (int)x0;
			return_val = sse_hvc_fclose_handler(x0);
			break;
	}

    // now we place 'return_val' into VM's pt_regs so it can see it.
    vm_regs->regs[0] = return_val;

}

int  sse_hvc_fopen_handler(const char* filename, const char* mode){

    printf("file: %s Mode: %s\n", filename, mode);

}
int  sse_hvc_fread_handler(char* buf, size_t size, size_t nmemb, int fd){

}
int  sse_hvc_fwrite_handler(char* buf, size_t size, size_t nmemb, int fd){

}
int  sse_hvc_fseek_handler(int fd, uint64_t offset, int pos){

}
int  sse_hvc_rewind_handler(int fd){

}
int  sse_hvc_fclose_handler(int fd){

}