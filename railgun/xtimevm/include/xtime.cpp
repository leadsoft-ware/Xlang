#include "../../xasm/include/xasm.cpp"
#include "error.cpp"
#include <cstdlib>
#include <cstring>
#define __WITH_DEVICES

/*
0号设备暂定为虚拟机，通过out(0,1)来关机
*/

#ifdef __WITH_DEVICES

#endif


#define regfp 16
#define regsp 17
#define regpc 18
#define intt  19 // 中断表

class virtual_machine{
    public:
    xasm::content regs[20];
    char* memory;int tot_mem;

    xasm::bytecode *this_byte(){
        return (xasm::bytecode*)(memory + regs[regpc].intval());
    }

    void next(){
        regs[regpc].intval() += sizeof(xasm::bytecode);
    }

    bool next_command(){
        if(this_byte() > (xasm::bytecode*)(memory + tot_mem)) return false;
        while(this_byte()->op != xasm::bytecode::_command) next();
        return true;
    }
    
    virtual_machine(xasm::xmvef_file file,size_t memsize){
        if(file.head.xmvef_sign != 0x114514ff) throw vm_error("Invalid Xmvef Sign");
        memory = (char*)malloc(memsize);
        // 初始化内存
        memcpy(memory,file.constant_pool,file.head.default_constant_pool_size);
        memcpy(memory + file.head.default_constant_pool_size,file.bytecode_array,sizeof(xasm::bytecode) * file.head.bytecode_length);
        regs[regfp] = 0;
        regs[regsp] = 0;
        regs[intt] = 0;
        regs[regpc] = file.head.start_of_pc;
    }

    // 返回地址，也可能是寄存器
    xasm::content* returnAddress(){
        if(this_byte()->op == xasm::bytecode::_address){
            return (xasm::content*)(memory + this_byte()->c.intval());
        }else if(this_byte()->op == xasm::bytecode::_addr_register){
            return (xasm::content*)(memory + regs[this_byte()->c.intval()].intval());
        }else if(this_byte()->op == xasm::bytecode::_register){
            return &regs[this_byte()->c.intval()];
        }
        return nullptr;
    }

    void start(){
        while(true){
            //xasm::bytecode* thisbyte = (xasm::bytecode*)( memory + (regs[regpc].intval() * sizeof(xasm::bytecode)) );
            if(this_byte()->op != xasm::bytecode::_command) throw vm_error("Not a command sign");
            if(xasm::cmdset[this_byte()->c.intval()] == "mov"){
                next(); // 移动pc
                xasm::content *dest = returnAddress(),src;
                next();
                src = ( returnAddress() != nullptr) ? *returnAddress() : this_byte()->c;
                *dest = src;
                // next(); // next和不next都行因为next_command的判断
            }else if(xasm::cmdset[this_byte()->c.intval()] == "mov_m"){
                next(); // same as last
                xasm::content *dest = returnAddress(),*src; // 不会吧不会吧，不会有人拿mov_m来复制常量吧？？？😅
                next();
                src = returnAddress();
                if(dest == nullptr || src == nullptr) throw vm_error("mov_m is not for constant");
                next();
                if(returnAddress() != nullptr) memcpy(dest,src,returnAddress()->intval());
                memcpy(dest,src,this_byte()->c.intval());
            }else if(xasm::cmdset[this_byte()->c.intval()] == "and"){
                next();
                xasm::content *dest = returnAddress(),src;
                if(dest == nullptr){throw vm_error("first augument must a address or register ");}
                next();
                if(returnAddress() != nullptr) src = *returnAddress();
                else src = this_byte()->c;
                *dest = dest->intval() & src.intval();
            }else if(xasm::cmdset[this_byte()->c.intval()] == "or"){
                next();
                xasm::content *dest = returnAddress(),src;
                if(dest == nullptr){throw vm_error("first augument must a address or register ");}
                next();
                if(returnAddress() != nullptr) src = *returnAddress();
                else src = this_byte()->c;
                *dest = dest->intval() | src.intval();
            }else if(xasm::cmdset[this_byte()->c.intval()] == "add" || xasm::cmdset[this_byte()->c.intval()] == "sub" || xasm::cmdset[this_byte()->c.intval()] == "mul" || xasm::cmdset[this_byte()->c.intval()] == "div" || xasm::cmdset[this_byte()->c.intval()] == "mod"){
                next();
                xasm::content *dest = returnAddress(),src;
                if(dest == nullptr){throw vm_error("first augument must a address or register ");}
                next();
                if(returnAddress() != nullptr) src = *returnAddress();
                else src = this_byte()->c;
                if(xasm::cmdset[this_byte()->c.intval()] == "add") *dest = dest->intval() + src.intval();
                if(xasm::cmdset[this_byte()->c.intval()] == "sub") *dest = dest->intval() - src.intval();
                if(xasm::cmdset[this_byte()->c.intval()] == "mul") *dest = dest->intval() * src.intval();
                if(xasm::cmdset[this_byte()->c.intval()] == "div") *dest = dest->intval() / src.intval();
                if(xasm::cmdset[this_byte()->c.intval()] == "mod") *dest = dest->intval() % src.intval();
            }else if(xasm::cmdset[this_byte()->c.intval()] == "_dbl_add" || xasm::cmdset[this_byte()->c.intval()] == "_dbl_sub" || xasm::cmdset[this_byte()->c.intval()] == "_dblmul" || xasm::cmdset[this_byte()->c.intval()] == "_dbl_div" || xasm::cmdset[this_byte()->c.intval()] == "_dbl_mod"){
                // 只处理小数
                next();
                xasm::content *dest = returnAddress(),src;
                if(dest == nullptr){throw vm_error("first augument must a address or register ");}
                next();
                if(returnAddress() != nullptr) src = *returnAddress();
                else src = this_byte()->c;
                if(xasm::cmdset[this_byte()->c.intval()] == "_dbl_add") *dest = dest->dblval() + src.dblval();
                if(xasm::cmdset[this_byte()->c.intval()] == "_dbl_sub") *dest = dest->dblval() - src.dblval();
                if(xasm::cmdset[this_byte()->c.intval()] == "_dbl_mul") *dest = dest->dblval() * src.dblval();
                if(xasm::cmdset[this_byte()->c.intval()] == "_dbl_div") *dest = dest->dblval() / src.dblval();
                //if(xasm::cmdset[this_byte()->c.intval()] == "_dbl_mod") *dest = dest->dblval() % src.dblval();
            }else if(xasm::cmdset[this_byte()->c.intval()] == "eq" || xasm::cmdset[this_byte()->c.intval()] == "neq" || 
                     xasm::cmdset[this_byte()->c.intval()] == "maxeq" || xasm::cmdset[this_byte()->c.intval()] == "mineq" ||
                     xasm::cmdset[this_byte()->c.intval()] == "max" || xasm::cmdset[this_byte()->c.intval()] == "min"){
                next();
                xasm::content *dest = returnAddress(),src1,src2;
                if(dest == nullptr){throw vm_error("first augument must a address or register ");}
                next();
                if(returnAddress() != nullptr) src1 = *returnAddress();
                src1 = this_byte()->c;
                next();
                if(returnAddress() != nullptr) src2 = *returnAddress();
                src2 = this_byte()->c;
                if(xasm::cmdset[this_byte()->c.intval()] == "eq") *dest = src1.intval() == src2.intval();
                if(xasm::cmdset[this_byte()->c.intval()] == "neq") *dest = src1.intval() != src2.intval();
                if(xasm::cmdset[this_byte()->c.intval()] == "maxeq") *dest = src1.intval() >= src2.intval();
                if(xasm::cmdset[this_byte()->c.intval()] == "mineq") *dest = src1.intval() <= src2.intval();
                if(xasm::cmdset[this_byte()->c.intval()] == "max") *dest = src1.intval() > src2.intval();
                if(xasm::cmdset[this_byte()->c.intval()] == "min") *dest = src1.intval() < src2.intval();
            }
            if( !next_command() ) break;
        }
    }
};