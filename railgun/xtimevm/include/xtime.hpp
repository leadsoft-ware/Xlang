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


/*
0号设备暂定为虚拟机，通过out(0,1)来关机
*/

// Xtime Standard Device Message Format
enum msgreq_type{_r_register,_w_register};
struct Xsdmf{
    char msgreq;
    xasm::content c;
};

struct device{
    xasm::content device_id;
    void *connect;  // function pointer connect(void* input_pipe)
    void *output_pipe;  // device_bridge(char* msg)
} port[0xff];

void* input_pipe(char* msg);