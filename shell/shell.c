#include "shell/shell.h"
#include "config.h"
#include "memory.h"
#include "mm/mm.h"
#include "stdio.h"
#include "string.h"
#include "core/vm.h"
#include "drivers/uart.h"
#include "stdlib.h"

// prototypes for shell commands
void list();
void enter();
void help();

struct shell* delta_shell;

uint8_t SHELL_ACTIVE_BEFORE = 0; // this is set to 0 when we go out of the shell to some other shell.
extern int total_vms;
extern struct vm* vmlist[CONFIG_MAX_VMs];
extern uint8_t vm_connected_to_uart;

struct shell_command{
    char command[CONFIG_MAX_SHELL_COMMAND_SIZE];  
    void (*command_handler)();
};

struct shell{
    uint64_t in;
    uint64_t out;
    char buffer[CONFIG_SHELL_BUFFER_SIZE];
    struct shell_command* commands[CONFIG_MAX_SHELL_COMMANDS];
};

static void add_shell_command(char command[], void (*command_handler)){
    int command_index;

    for(command_index = 0; command_index < CONFIG_MAX_SHELL_COMMANDS; command_index++){
        if(delta_shell->commands[command_index] == 0) break;
    }

    if(command_index == CONFIG_MAX_SHELL_COMMANDS){
        printf("shell command %s not added. max limit reached\n", command);
        return;
    }

    delta_shell->commands[command_index] = (struct shell_command*) malloc(sizeof(struct shell_command));
    strcpy(delta_shell->commands[command_index]->command, command);
    delta_shell->commands[command_index]->command_handler = command_handler;
}

void shell_run(){
    if(!SHELL_ACTIVE_BEFORE){
        printf(">>> ");
        SHELL_ACTIVE_BEFORE = 1;
    }
    char shell_input[CONFIG_MAX_SHELL_COMMAND_SIZE];
    gets(shell_input);

    uint8_t command_recognised = 0;

    for(int command_index=0; command_index < CONFIG_MAX_SHELL_COMMANDS; command_index++){
        if(delta_shell->commands[command_index] != 0 && !strcmp(shell_input, delta_shell->commands[command_index]->command)){
            delta_shell->commands[command_index]->command_handler();
            command_recognised = 1;
        }
    }

    if(!command_recognised) printf("command not recognised.\n");
    printf(">>> ");
}

void shell_init(){
    struct shell* delta_shell = (struct shell*) malloc(sizeof(struct shell));
    memset(delta_shell, 0, sizeof(delta_shell));

    add_shell_command("list", list);
    add_shell_command("enter", enter);
    add_shell_command("help", help);

    log("Shell initialised\n");
}

// ===================================== Shell Commands =================================================

void list(){
    for(int vm=0;vm<total_vms;vm++){
        printf("VMID: %d\n", vmlist[vm]->vmid);
        printf("Name: %s\n", vmlist[vm]->name);
        printf("State: %s\n", vm_state_to_string(vmlist[vm]->state));
        printf("Size: %d\n", vmlist[vm]->vmdata_size);
        printf("\n");
    }
}

void enter(){
 
}

void help(){
    printf("\n");
    printf("list: \tList all VMs and their status.\n");
    printf("enter: \tEnter a VM.\n");
    printf("help: \tList all commands and their functions.\n");
    printf("\n");
}