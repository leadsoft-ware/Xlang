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
struct DevicePackage{
    std::vector<int> require_int_num;
    char device_name[32];
};

// #if defined __ENABLE_BACKTRACE
std::stack<std::string> backtrace;
// endif


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
    VMExec vmexec;
    device_host device_bridge;
    Content regs[32];
    PC_Register pc;
    std::map<std::string,bool> vm_rules;
    bool    regflag;
    char* malloc_place;
    TSS* thisTSS;
    VMRuntime(VMExec& vme,int fd){
        vmexec = vme;
        malloc_place = (char*) malloc(vme.cpool.size * 3 + vme.head.code_length * sizeof(ByteCode));
        char* memtop = malloc_place;
        // fill constant pool 
        memcpy(memtop,vmexec.cpool.pool,vmexec.cpool.size);
        memtop += vmexec.cpool.size;
        thisTSS = (TSS*) memtop;
        thisTSS->basememory.intc = 0;
        thisTSS->beforceTSS.intc = pointerSubtract(thisTSS , malloc_place);
        thisTSS->fp.intc = 0;
        thisTSS->sp.intc = 0;
        thisTSS->pc.intc = 0;
        memtop += sizeof(TSS);
        thisTSS->vstack_start.intc = pointerSubtract(memtop , malloc_place);
    }
    void StartMainLoop(){

    }
    void Run(){

    }
};

void DebugOutput(VMRuntime rt, std::ostream &out = std::cout){
    out << "==========================[Debug Output]==========================\n";
    for(int i = 0;i < 32;i=i+1){
        if(i < 10) out << "reg" << i << " : " << rt.regs[i].intc << " ";
        else out << "reg" << i << ": " << rt.regs[i].intc << " ";
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