#include "../../xasm/include/xasm.cpp"
#include "error.cpp"

#include <cstdlib>
#include <cstring>
#define __WITH_DEVICES

#define regfp 16
#define regsp 17
#define regpc 18
#define intt  19 // 中断表

// Xtime virt: 与主虚拟机隔离的虚拟机（套娃）
struct virt_task{
    xasm::content base; // 内存基址
    xasm::content total_memory;
    xasm::content regs[20];
};

class virtual_machine{
    public:
    virt_task *main_task;
    char* memory;int tot_mem;

    xasm::bytecode *this_byte();
    void next();
    bool next_command();
    virtual_machine(xasm::xmvef_file file,size_t memsize);
    xasm::content* returnAddress();
    long long getStackRealAddress();
    void start();
};


#ifdef __WITH_DEVICES

#include "dlfcn.h"

/*
0号设备暂定为虚拟机，通过out(0,1)来关机
*/

typedef void(* _dconnect)(virtual_machine*);
typedef void(* _dinput)(char*); // 从设备获取输入
typedef void(* _doutput)(char*); // 输出到设备

struct device{
    xasm::content devid;
    _dconnect connect;
    _dinput input;
    _doutput output;
}port[0xff];
void* device_handle[0xff];
int tot_dev_cnt;

void device_plug_in(char* path);

#endif