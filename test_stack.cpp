#include "vm/core.cpp"
using namespace std;

int main(){
    string alloc;
    alloc.resize(1024);
    TSS default_task;
    default_task.basememory.intc = 0;
    default_task._AllocSize.intc = 1024;
    default_task.vstack_start.intc = 1024;
    default_task.fp.intc = 0;
    default_task.sp.intc = 0;
    vm_stack s((char*)alloc.data(),&default_task);
    s.save();
    
}