#include "shell/shell.h"
#include "config.h"
#include "memory.h"
#include "mm/mm.h"
#include "stdio.h"
#include "string.h"

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

static void add_shell_command(struct shell* _shell, char command[], void (*command_handler)){
    int command_index;

    for(command_index = 0; command_index < CONFIG_MAX_SHELL_COMMANDS; command_index++){
        if(_shell->commands[command_index] == 0) break;
    }

    if(command_index == CONFIG_MAX_SHELL_COMMANDS){
        printf("shell command %s not added. max limit reached\n", command);
        return;
    }

    _shell->commands[command_index] = (struct shell_command*) malloc(sizeof(struct shell_command));
    strcpy(_shell->commands[command_index], command);
    _shell->commands[command_index]->command_handler = command_handler;
}

void shell_run(struct shell* _shell){
    printf(">>> ");
    char shell_input[CONFIG_MAX_SHELL_COMMAND_SIZE];
    gets(shell_input);

    for(int command_index=0; command_index < CONFIG_MAX_SHELL_COMMANDS; command_index++){
        if(_shell->commands[command_index] != 0 && (shell_input, _shell->commands[command_index]->command)){
            _shell->commands[command_index]->command_handler();
            shell_run(_shell);
        }
    }

    printf("shell command not recognised.\n");
    shell_run(_shell);
}

void shell_init(){
    struct shell* _shell = (struct shell*) malloc(sizeof(struct shell));
    _shell->in = 0;
    _shell->out = 0;
    memset(_shell->buffer, 0, sizeof(_shell->buffer));
    memset(_shell->commands, 0, sizeof(_shell->commands));

    shell_run(_shell);

}

