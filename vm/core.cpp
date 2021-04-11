/*
  git config --global user.email "3134714226@qq.com"
  git config --global user.name "Xlang_Xiaokang00010"
*/
#pragma once
#include <iostream>
#include <bits/stdc++.h>
#include <unistd.h>
#include <fcntl.h>
#include "../compiler/basic_type.cpp"
#include "../lib/XVMDK/dlfcn.h"
#include "const.cpp"

typedef unsigned long int addr_t;

class VMError{
    public:
    std::string str;
    VMError(std::string msg){
        str = msg;
    }
    void what(){
        std::cerr << "\033[31m[VMERR]\033[0m " << str << "\n";
    }
};

struct ByteCode{
    char opid;
    Content c;
};

struct CodeLabel{
    char label_n[32]; // Label name
    int label_id;   // for jmp command
    long int start; // start position
};

struct AuthorMessage{
    char AuthorName[32];
    char ProgramName[32];
    char major,minjor,build;
};

struct VMExecHeader{
    long int hash; // default is 0x114514ff
    short support_vm_build; // support vm build version
    long int code_label_count; // if this is zero , throw Error
    size_t code_length;
    AuthorMessage a_msg;
};

int s=sizeof(CodeLabel);

struct ConstantPool{
    addr_t size,count;
    size_t* items;
    char* pool;
};

struct VMExec{
    VMExecHeader head;
    CodeLabel* label_array;
    ByteCode* code_array;
    ConstantPool cpool;
};

void VMExec_Serialization(char* filename,VMExec vme){
    int fd = open(filename,O_RDWR|O_CREAT,0777);
    if(fd == -1) perror("File open error!");
    write(fd,&vme.head,sizeof(VMExecHeader));

    write(fd,vme.label_array,sizeof(CodeLabel) * vme.head.code_label_count);
    write(fd,vme.code_array,sizeof(ByteCode) * vme.head.code_length);

    write(fd,&vme.cpool.size,8);
    write(fd,vme.cpool.pool,vme.cpool.size);
    close(fd);
}

// return an not closed file handle for constant pool init
int LoadVMExec(char* filename,VMExec& vme){
    int fd = open(filename,O_RDWR|O_CREAT,0777);
    //vme->cpool = (ConstantPool*)malloc(sizeof(ConstantPool));
    read(fd,&vme.head,sizeof(VMExecHeader));

    vme.label_array = (CodeLabel*)malloc(sizeof(CodeLabel) * vme.head.code_label_count);
    vme.code_array = (ByteCode*)malloc(sizeof(ByteCode) * vme.head.code_length);
    read(fd,vme.label_array,sizeof(CodeLabel) * vme.head.code_label_count);
    read(fd,vme.code_array,sizeof(ByteCode) * vme.head.code_length);

    read(fd,&vme.cpool.size,8);
    return fd;
}

struct heap_item_t{
    addr_t c[3];
    addr_t& operator[](int index){
        return c[index];
    }
};

struct TSS{
    Content beforceTSS;
    Content fp,sp;
    Content pc;
    bool    regflag;
    Content regs[32];
    Content basememory;
    Content custom_code_labels;
    Content vstack_start;
    Content _AllocSize;
};

enum opid_list{
    UnusualRegister = 1,
    NormalRegister  = 2,
    Command         = 3,
    Number          = 4,
    Address         = 5,
    Address_Register= 6,
} opid_kind;
std::string COMMAND_MAP[] = {
    "mov","mov_m","push","pop","save","pop_frame",
    "add","sub","mul","div",
    "equ","neq","maxeq","mineq","max","min",
    "jmp","jt","jf","call",
    "exit","ret","in","out","req","push1b","restore","fork",
    "tclear","tset","trestore", // 分别为清除中断处理状态，设置中断处理状态，返回主任务
    "labelg","labels", // label-get label-set
};
std::map<std::string,long> realmap;
class PC_Register{
    public:
    std::string* basestr;
    ByteCode* offset;
    long current_offset;
    PC_Register(){};
    PC_Register(std::string& basestr,void* start){
        this->basestr = &basestr;
        offset = (ByteCode*)start;
    }
    void operator++(int x){
        while(true){
            offset++;
            if(offset->opid == Command) break;
        }
    }
    void operator--(int x){
        while(true){
            offset--;
            if(offset->opid == Command) break;
        }
    }
    void operator+=(long c){
        //long  = 0;
        if(c > 0) for(int i = 0;i < c;i++) (*this)++;
        if(c < 0) for(int i = 0;i < 0-c;i++) (*this)--;
    }
    void operator=(ByteCode* pos){
        offset = pos;
    }
};

class InterrputControler{
public:
    long HasInterrputSignal;
    bool IsProcessingSignal;
    std::vector<Content> RegisteredProcessingFunction;
};

struct Device{
    char device_name[32];
    void* io_request;
    void* normal_request;
};

// #if defined __ENABLE_BACKTRACE
std::stack<std::string> backtrace;
// endif

class vm_stack{
    public:
    char* basememory;
    TSS* task;
    vm_stack(){}
    vm_stack(char* basemem,TSS* task){
        basememory = basemem;
        this->task = task;
    }
    void push(char* data,long size){
        task->sp.intc += size;
        char* realaddr = basememory + task->basememory.intc + task->_AllocSize.intc - task->fp.intc - task->sp.intc;
        memcpy(realaddr,data,size);
    }
    void push(Content& s){
        this->push((char*)&s.chc,8);
    }
    char* pop(long size){
        char* ret = basememory + task->basememory.intc + task->_AllocSize.intc - task->fp.intc - task->sp.intc;
        task->sp.intc -= size;
        return ret;
    }
    Content* pop(){
        return (Content*)pop(8);
    }
    void save(){
        for(int i = 0;i < 32;i++){
            push(task->regs[i]);
        }
        push(task->pc);
        push((char*)&task->regflag,1);
        push(task->fp);
        push(task->sp);
        task->fp.intc += task->sp.intc;
        task->sp.intc = 0;
    }
    void restore(){
        pop(task->sp.intc);
        Content temp_sp = *pop();
        Content temp_fp = *pop();
        std::cout << temp_fp.intc << " " << temp_sp.intc << std::endl;
        task->sp = temp_sp;
        task->sp.intc -= 16; // 恢复至未压入sp,fp的状态
        task->fp = temp_fp;
        task->regflag = *pop(1);
        task->pc = *pop();
        for(int i=31;i >= 0;i--){
            task->regs[i] = *pop();
        }
        std::cout << task->fp.intc << " " << task->sp.intc << std::endl;
    }
};

#define __XVMDK_HOST
#include "../lib/XVMDK/framework.cpp"

struct device_host{
    std::vector<Device> devlist;
    std::vector<void*>  devhandle;
    
    int LoadVMDevice(void* env,std::string path){
        devhandle.push_back(dlopen(path.c_str(),RTLD_LAZY));
        if(devhandle[devhandle.size() - 1] == nullptr) throw VMError("Load Device Failed:" + path);
        Device dev;
        dev.io_request = dlsym(devhandle[devhandle.size() - 1],"io_request"),sizeof(void*);
        dev.normal_request = dlsym(devhandle[devhandle.size() - 1],"normal_request"),sizeof(void*);
        char* (*f_device_load)(void*) = (char* (*)(void*))dlsym(devhandle[devhandle.size() - 1],"on_device_load");
        char* result = f_device_load(env);
        memcpy(dev.device_name,result,32);
        devlist.push_back(dev);
        return devhandle.size() - 1;
    }

    void device_in(long target,void* env,char* dest){
        void (*io_func)(void* env,char motd,char* dest) = (void (*)(void*,char,char*)) devlist[target].io_request;
        io_func(env,0/*in*/,dest);
    }
    void device_out(long target,void* env,char* dest){
        void (*io_func)(void* env,char motd,char* dest) = (void (*)(void*,char,char*)) devlist[target].io_request;
        io_func(env,1/*out*/,dest);
    }
    void device_request(long target,void* env,char reqid,char* dest){
        void (*normalreq)(void* env,char reqid,char* dest) = (void (*)(void*,char,char*)) devlist[target].normal_request;
        normalreq(env,reqid,dest);
    }
};


device_host devhost;

class VMRuntime{
    public:
    long Alloc_Size;
    VMExec vmexec;
    vm_stack stack;
    device_host device_bridge;
    InterrputControler intc;
    //Content regs[32];
    PC_Register pc;
    std::map<std::string,bool> vm_rules;
    bool    regflag;
    char* malloc_place;
    TSS* thisTSS;

    // 反编译当前语句
    void disasm(){
        std::cout << COMMAND_MAP[pc.offset->c.intc] << " ";
        
    }
    char* getAddress(ByteCode t){
        if(t.opid == NormalRegister){
            return (char*)&thisTSS->regs[t.c.intc];
        }else if(t.opid == UnusualRegister){
            if(t.c.intc == 0) return (char*)&thisTSS->fp;// fp
            if(t.c.intc == 1) return (char*)&thisTSS->sp;// sp
            if(t.c.intc == 3) return (char*)&Alloc_Size;// sb
        }else if(t.opid == Address){
            return (char*)malloc_place + thisTSS->basememory.intc + t.c.intc;
        }else if(t.opid == Address_Register){
            return (char*)malloc_place + thisTSS->basememory.intc + thisTSS->regs[t.c.intc].intc;
        }
        return nullptr;
    }
    
    VMRuntime(VMExec& vme,int fd){
        vmexec = vme;
        malloc_place = (char*) malloc(vme.cpool.size * 3 + vme.head.code_length * sizeof(ByteCode) + sizeof(TSS));
        char* memtop = malloc_place;
        // fill constant pool 
        memcpy(memtop,vmexec.cpool.pool,vmexec.cpool.size);
        memtop += vmexec.cpool.size;
        memcpy(memtop,vmexec.code_array,vmexec.head.code_length * sizeof(ByteCode));
        memtop += vmexec.head.code_length * sizeof(ByteCode);
        thisTSS->pc.intc = pointerSubtract(memtop , malloc_place);
        thisTSS = (TSS*) memtop;
        thisTSS->basememory.intc = 0;
        thisTSS->beforceTSS.intc = pointerSubtract(thisTSS , malloc_place);
        thisTSS->fp.intc = 0;
        thisTSS->sp.intc = 0;
        memtop += sizeof(TSS);
        thisTSS->vstack_start.intc = vme.cpool.size * 3 + vme.head.code_length * sizeof(ByteCode) + sizeof(TSS) - 1; // 从上往下
        Alloc_Size = vme.cpool.size * 3 + vme.head.code_length * sizeof(ByteCode) + sizeof(TSS) - 1;
        stack = vm_stack(malloc_place,thisTSS);
    }
    void SeekToStart(){
        long Origin;
        for(int i = 0;i < vmexec.head.code_label_count;i++){
            if(std::string(vmexec.label_array[i].label_n) == "_vmstart") Origin = i;
        }
        pc.offset = (ByteCode*)malloc_place + thisTSS->basememory.intc + thisTSS->pc.intc + vmexec.label_array[Origin].start;
    }
    void StartMainLoop(){
        SeekToStart();
        while(COMMAND_MAP[pc.offset->c.intc] != "exit"){
            if(intc.HasInterrputSignal){
            
            }
            if(COMMAND_MAP[pc.offset->c.intc] == "mov"){
                char* movto = getAddress(*(pc.offset+1));
                if(movto == nullptr) throw VMError("CommandError: Move Command must have a address to save data");
                Content source;
                if(getAddress(*(pc.offset+2)) != nullptr) source = *(Content*)getAddress(*(pc.offset+2));
                else source = (pc.offset+2)->c;
                *(Content*)movto = source;
            }
            if(COMMAND_MAP[pc.offset->c.intc] == "mov_m"){
                char* movto = getAddress(*(pc.offset+1));char* src = getAddress(*(pc.offset+2));long size = (pc.offset+3)->c.intc;
                if(movto == nullptr) throw VMError("CommandError: Move Command must have a address to save data");
                if(src == nullptr){
                    *(Content*)movto = (pc.offset+2)->c;
                }else{
                    for(int i = 0;i < size;i++) movto[i] = src[i];
                }
            }
            if(COMMAND_MAP[pc.offset->c.intc] == "push"){
                char* src = getAddress(*(pc.offset+1));long size = (pc.offset+2)->c.intc;
                if(src == nullptr) throw VMError("CommandError: the first arg of push must be a address or register");
                stack.push(src,size);
            }
            if(COMMAND_MAP[pc.offset->c.intc] == "pop"){
                char* src = getAddress(*(pc.offset+1));long size = (pc.offset+2)->c.intc;
                if(src == nullptr) throw VMError("CommandError: the first arg of pop must be a address or register");
                memcpy(src,stack.pop(size),size);
            }
            if(COMMAND_MAP[pc.offset->c.intc] == "save"){
                // 将所有寄存器的内容保存至栈中，并且开新帧
                stack.save();
            }
            if(COMMAND_MAP[pc.offset->c.intc] == "pop_frame"){
                stack.restore();
            }
        }
    }
    void Run(){
        if(vm_rules["verbose"] == true){
            std::cout << "Xlang " << __VM_VERSION << " Build:" << __VM_BUILD << " Codename:" << __VM_CODENAME << std::endl;
        }
        StartMainLoop();
    }
};

void DebugOutput(VMRuntime rt, std::ostream &out = std::cout){
    out << "==========================[Debug Output]==========================\n";
    for(int i = 0;i < 32;i=i+1){
        if(i < 10) out << "reg" << i << " : " << rt.thisTSS->regs[i].intc << " ";
        else out << "reg" << i << ": " << rt.thisTSS->regs[i].intc << " ";
        if(i % 7 == 0 && i != 0) out <<  std::endl; 
    }
    out << "\n";
    out << "REGFLAG:" << rt.regflag << " PC:" << rt.pc.offset <<  std::endl;
    out << "==========================[EndOf Output]==========================\n";
}

void Memory_Watcher(VMRuntime rt,long v,std::ostream &out = std::cout){
    Content *c = (Content*)(&(rt.malloc_place + rt.thisTSS->basememory.intc)[v]);
    out << "Memory Watcher=>" << v << "\n int val=>" << c->intc << "\n Charter Val=>" << std::string(c->chc,8) << "\n";
}