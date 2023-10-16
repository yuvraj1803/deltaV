#include "shell/shell.h"
#include "config.h"
#include "memory.h"
#include "mm/mm.h"
#include "stdio.h"
#include "string.h"

struct shell* delta_shell;

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
    printf(">>> ");
    char shell_input[CONFIG_MAX_SHELL_COMMAND_SIZE];
    gets(shell_input);

    for(int command_index=0; command_index < CONFIG_MAX_SHELL_COMMANDS; command_index++){
        if(delta_shell->commands[command_index] != 0 && strcmp(shell_input, delta_shell->commands[command_index]->command)){
            delta_shell->commands[command_index]->command_handler();
            shell_run(delta_shell);
        }
    }

    printf("shell command not recognised.\n");
}

void shell_init(){
    struct shell* delta_shell = (struct shell*) malloc(sizeof(struct shell));
    delta_shell->in = 0;
    delta_shell->out = 0;
    memset(delta_shell->buffer, 0, sizeof(delta_shell->buffer));
    memset(delta_shell->commands, 0, sizeof(delta_shell->commands));
    log("Shell initialised\n");
}

