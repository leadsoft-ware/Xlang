#include "../include/xtime.hpp"
#include "../../include/systemapis.cpp"

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
        source[0] = getch();
        break;

    // read 8 byte
    case 1:
        for(int i = 0;i < 8;i++) source[i] = getch();
        break;

    // read 16 byte
    case 2:
        for(int i = 0;i < 16;i++) source[i] = getch();
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