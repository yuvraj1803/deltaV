#include "core/console.h"

void console_push(struct vm_console* console, int val){
    if(console_full(console)) return;

    console->buffer[console->end] = val;
    console->end = (console->end + 1)%CONFIG_VM_CONSOLE_BUFFER_SIZE;
    console->used++;
}

int console_pop(struct vm_console* console){
    if(console_empty(console)) return -1;

    int out = console->buffer[console->begin];
    console->begin = (console->begin + 1) % CONFIG_VM_CONSOLE_BUFFER_SIZE;
    console->used--;

    return out & 0xff;
}

int console_full(struct vm_console* console){
    return console->used == CONFIG_VM_CONSOLE_BUFFER_SIZE;
}

int console_empty(struct vm_console* console){
    return console->used == 0;
}

void console_clear(struct vm_console* console){
    console->begin = 0;
    console->end = 0;
    console->used = 0;
}

void console_print(struct vm_console* console){
    int ptr = console->begin;
    while(console->buffer[ptr] && ptr != console->end){
        printf("%c", console->buffer[ptr]);
        ptr = (ptr + 1)%CONFIG_VM_CONSOLE_BUFFER_SIZE;
    }
}