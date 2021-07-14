#include "../include/xtime.hpp"

#define export extern "C"

virtual_machine *machine;

export void connect(virtual_machine* vm){
    machine = vm;
}

export void input(char* source){
    switch (machine->main_task->regs[0].intval())
    {
    // standard input (read one byte)
    case 0:
        read(0,source,1);
        break;

    // read 8 byte
    case 1:
        read(0,source,8);
        break;

    // read 16 byte
    case 2:
        read(0,source,16);
        break;
    
    default:
        break;
    }
}

export void output(char* source){
    switch (machine->main_task->regs[0].intval())
    {
    // out 1 byte
    case 0:
        write(1,source,1);

    // out 8 byte
    case 1:
        write(1,source,8);

    // out 16 byte
    case 2:
        write(2,source,16);
    }
}