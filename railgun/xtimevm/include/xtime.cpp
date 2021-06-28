#include "xtime.hpp"

xasm::bytecode *virtual_machine::this_byte(){
    return (xasm::bytecode*)(memory + main_task->base.intval() + main_task->regs[regpc].intval());
}

void virtual_machine::next(){
    main_task->regs[regpc].intval() += sizeof(xasm::bytecode);
}

bool virtual_machine::next_command(){
    if((char*)this_byte() > (memory + main_task->total_memory.intval())) return false;
    while(this_byte()->op != xasm::bytecode::_command) next();
    return true;
}
    
virtual_machine::virtual_machine(xasm::xmvef_file file,size_t memsize){
    if(file.head.xmvef_sign != 0x114514ff) throw vm_error("Invalid Xmvef Sign");
    memory = (char*)malloc(memsize);
    tot_mem = memsize;
    // 初始化内存
    memcpy(memory,file.constant_pool,file.head.default_constant_pool_size);
    memcpy(memory + file.head.default_constant_pool_size,file.bytecode_array,sizeof(xasm::bytecode) * file.head.bytecode_length);
    main_task = (virt_task*)(memory + file.head.default_constant_pool_size+sizeof(xasm::bytecode) * file.head.bytecode_length);
    main_task->regs[regfp] = 0;
    main_task->regs[regsp] = 0;
    main_task->regs[intt] = 0;
    main_task->regs[regpc] = file.head.start_of_pc;
    main_task->base.intval() = 0;
    main_task->total_memory.intval() = tot_mem;
}

    // 返回地址，也可能是寄存器
xasm::content* virtual_machine::returnAddress(){
    if(this_byte()->op == xasm::bytecode::_address){
        return (xasm::content*)(memory + main_task->base.intval() + this_byte()->c.intval());
    }else if(this_byte()->op == xasm::bytecode::_addr_register){
        return (xasm::content*)(memory + main_task->base.intval() + main_task->regs[this_byte()->c.intval()].intval());
    }else if(this_byte()->op == xasm::bytecode::_register){
        return &main_task->regs[this_byte()->c.intval()];
    }
    return nullptr;
}

long long virtual_machine::getStackRealAddress(){
    return main_task->total_memory.intval() - main_task->regs[regfp].intval() - main_task->regs[regsp].intval();
}

void virtual_machine::start(){
    while(true){
        //xasm::bytecode* thisbyte = (xasm::bytecode*)( memory + (regs[regpc].intval() * sizeof(xasm::bytecode)) );
        //if(this_byte()->op == xasm::bytecode::_command){ std::cout << xasm::cmdset[this_byte()->c.intval()] << "(";}
        
        if(this_byte()->op != xasm::bytecode::_command) throw vm_error("Not a command sign");
        std::cout << xasm::cmdset[this_byte()->c.intval()] << " ";
        for(int i =1;( main_task->regs[18].intval() < main_task->total_memory.intval() ) && ( (this_byte() + i)->op != xasm::bytecode::_command );i++){
            if((this_byte()+i)->op == xasm::bytecode::_addr_register) std::cout << "*reg" << (this_byte()+i)->c.intval() ;
            if((this_byte()+i)->op == xasm::bytecode::_address) std::cout << "*" << (this_byte()+i)->c.intval() ;
            if((this_byte()+i)->op == xasm::bytecode::_number) std::cout << (this_byte()+i)->c.intval() ;
            if((this_byte()+i)->op == xasm::bytecode::_register) std::cout << "reg" << (this_byte()+i)->c.intval() ;
            std::cout << ",";
        }
        std::cout << "\b;\n";
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
            int cmd = this_byte()->c.intval();
            next();
            xasm::content *dest = returnAddress(),src;
            if(dest == nullptr){throw vm_error("first augument must a address or register ");}
            next();
            if(returnAddress() != nullptr) src = *returnAddress();
            else src = this_byte()->c;
            if(xasm::cmdset[cmd] == "add") dest->intval() = dest->intval() + src.intval();
            else if(xasm::cmdset[cmd] == "sub") dest->intval() = dest->intval() - src.intval();
            else if(xasm::cmdset[cmd] == "mul") dest->intval() = dest->intval() * src.intval();
            else if(xasm::cmdset[cmd] == "div") dest->intval() = dest->intval() / src.intval();
            else if(xasm::cmdset[cmd] == "mod") dest->intval() = dest->intval() % src.intval();
        }else if(xasm::cmdset[this_byte()->c.intval()] == "_dbl_add" || xasm::cmdset[this_byte()->c.intval()] == "_dbl_sub" || xasm::cmdset[this_byte()->c.intval()] == "_dblmul" || xasm::cmdset[this_byte()->c.intval()] == "_dbl_div" || xasm::cmdset[this_byte()->c.intval()] == "_dbl_mod"){
            int cmd = this_byte()->c.intval();
            // 只处理小数
            next();
            xasm::content *dest = returnAddress(),src;
            if(dest == nullptr){throw vm_error("first augument must a address or register ");}
            next();
            if(returnAddress() != nullptr) src = *returnAddress();
            else src = this_byte()->c;
            if(xasm::cmdset[cmd] == "_dbl_add") dest->dblval() = dest->dblval() + src.dblval();
            if(xasm::cmdset[cmd] == "_dbl_sub") dest->dblval() = dest->dblval() - src.dblval();
            if(xasm::cmdset[cmd] == "_dbl_mul") dest->dblval() = dest->dblval() * src.dblval();
            if(xasm::cmdset[cmd] == "_dbl_div") dest->dblval() = dest->dblval() / src.dblval();
            //if(xasm::cmdset[this_byte()->c.intval()] == "_dbl_mod") *dest = dest->dblval() % src.dblval();
        }else if(xasm::cmdset[this_byte()->c.intval()] == "eq" || xasm::cmdset[this_byte()->c.intval()] == "neq" || 
                 xasm::cmdset[this_byte()->c.intval()] == "maxeq" || xasm::cmdset[this_byte()->c.intval()] == "mineq" ||
                 xasm::cmdset[this_byte()->c.intval()] == "max" || xasm::cmdset[this_byte()->c.intval()] == "min"){
            int cmd = this_byte()->c.intval();
            next();
            xasm::content *dest = returnAddress(),src1,src2;
            if(dest == nullptr){throw vm_error("first augument must a address or register ");}
            next();
            if(returnAddress() != nullptr) src1 = *returnAddress();
            src1 = this_byte()->c;
            next();
            if(returnAddress() != nullptr) src2 = *returnAddress();
            src2 = this_byte()->c;
            if(xasm::cmdset[cmd] == "eq") dest->intval() = src1.intval() == src2.intval();
            if(xasm::cmdset[cmd] == "neq") dest->intval() = src1.intval() != src2.intval();
            if(xasm::cmdset[cmd] == "maxeq") dest->intval() = src1.intval() >= src2.intval();
            if(xasm::cmdset[cmd] == "mineq") dest->intval() = src1.intval() <= src2.intval();
            if(xasm::cmdset[cmd] == "max") dest->intval() = src1.intval() > src2.intval();
            if(xasm::cmdset[cmd] == "min") dest->intval() = src1.intval() < src2.intval();
        }else if(xasm::cmdset[this_byte()->c.intval()] == "_itd" || xasm::cmdset[this_byte()->c.intval()] == "_dti"){
            int cmd = this_byte()->c.intval();
            next();
            xasm::content *dest = returnAddress();
        if(dest == nullptr){throw vm_error("first augument must a address or register");}
            if(xasm::cmdset[cmd] == "_itd") dest->dblval() = dest->intval();
            else dest->intval() = dest->dblval();
        }else if(xasm::cmdset[this_byte()->c.intval()] == "mov1b"){
            next();
            xasm::content *dest = returnAddress(),src;
            if(dest == nullptr){throw vm_error("first augument must a address or register");}
            next();
            if(returnAddress() != nullptr){src = *returnAddress();}
            else src = this_byte()->c;
            next();
            char getbit = this_byte()->c.intval();
            dest->intval() = src.charval()[getbit];
        }else if(xasm::cmdset[this_byte()->c.intval()] == "push" || xasm::cmdset[this_byte()->c.intval()] == "pop"){
            int cmd = this_byte()->c.intval();
            next();
            xasm::content dest1,*dest = returnAddress(),size;
            if(dest == nullptr) dest1 = this_byte()->c;
            next();
            if(returnAddress() != nullptr){size = *returnAddress();}
            else size = this_byte()->c;
            if(xasm::cmdset[cmd] == "push"){
                if(dest == nullptr){
                    main_task->regs[regsp].intval() += 8;
                    *((xasm::content*)(memory + main_task->base.intval() + getStackRealAddress())) = dest1;
                }else{
                    main_task->regs[regsp].intval() += size.intval();
                    for(int i = 0;i < size.intval();i++){
                        (memory + main_task->base.intval() + getStackRealAddress())[i] = dest->charval()[i];
                    }
                }
            }else{
                if(dest == nullptr){throw vm_error("pop command return a data address");}
                dest->intval() = getStackRealAddress(); // 因为只给地址，所以不用太在意加上base
                main_task->regs[regsp].intval() -= size.intval();
            }
        }
        if( !next_command() ) break;
    }
}

void device_plug_in(char* path){
    device_handle[tot_dev_cnt++] = dlopen(path,RTLD_LAZY);
    if(device_handle[tot_dev_cnt-1] == nullptr) throw vm_error("device not currently loaded.");
    port[tot_dev_cnt-1].connect = (_dconnect)dlsym(device_handle[tot_dev_cnt-1],"connect");
    port[tot_dev_cnt-1].input = (_dinput)dlsym(device_handle[tot_dev_cnt-1],"input");
    port[tot_dev_cnt-1].output = (_doutput)dlsym(device_handle[tot_dev_cnt-1],"output");
    if(port[tot_dev_cnt-1].connect == nullptr || port[tot_dev_cnt-1].input == nullptr || port[tot_dev_cnt-1].output == nullptr) throw vm_error("device not currently loaded.");
}