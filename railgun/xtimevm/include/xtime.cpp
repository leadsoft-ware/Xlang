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

bool virtual_machine::check_cmd(std::string current,long s){
    return xasm::cmdset[s == -1 ? this_byte()->c.intval() : s] == current;
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
        if(check_cmd("mov")){
            next(); // 移动pc 
            xasm::content *dest = returnAddress(),src;
            next();
            src = ( returnAddress() != nullptr) ? *returnAddress() : this_byte()->c;
            *dest = src;
            // next(); // next和不next都行因为next_command的判断
        }else if(check_cmd("mov_m")){
            next(); // same as last
            xasm::content *dest = returnAddress(),*src; // 不会吧不会吧，不会有人拿mov_m来复制常量吧？？？😅
            next();
            src = returnAddress();
            if(dest == nullptr || src == nullptr) throw vm_error("mov_m is not for constant");
            next();
            if(returnAddress() != nullptr) memcpy(dest,src,returnAddress()->intval());
            memcpy(dest,src,this_byte()->c.intval());
        }else if(check_cmd("and")){
            next();
            xasm::content *dest = returnAddress(),src;
            if(dest == nullptr){throw vm_error("first augument must a address or register ");}
            next();
            if(returnAddress() != nullptr) src = *returnAddress();
            else src = this_byte()->c;
            *dest = dest->intval() & src.intval();
        }else if(check_cmd("or")){
            next();
            xasm::content *dest = returnAddress(),src;
            if(dest == nullptr){throw vm_error("first augument must a address or register ");}
            next();
            if(returnAddress() != nullptr) src = *returnAddress();
            else src = this_byte()->c;
            *dest = dest->intval() | src.intval();
        }else if(check_cmd("add") || check_cmd("sub") || check_cmd("mul") || check_cmd("min") || check_cmd("mod")){
            int cmd = this_byte()->c.intval();
            next();
            xasm::content *dest = returnAddress(),src;
            if(dest == nullptr){throw vm_error("first augument must a address or register ");}
            next();
            if(returnAddress() != nullptr) src = *returnAddress();
            else src = this_byte()->c;
            if(check_cmd("add",cmd)) dest->intval() = dest->intval() + src.intval();
            else if(check_cmd("sub",cmd)) dest->intval() = dest->intval() - src.intval();
            else if(check_cmd("mul",cmd)) dest->intval() = dest->intval() * src.intval();
            else if(check_cmd("div",cmd)) dest->intval() = dest->intval() / src.intval();
            else if(check_cmd("mod",cmd)) dest->intval() = dest->intval() % src.intval();
        }else if(check_cmd("_dbl_add") || check_cmd("_dbl_sub") || check_cmd("_dbl_mul") || check_cmd("_dbl_div") || check_cmd("_dbl_mod")){
            int cmd = this_byte()->c.intval();
            // 只处理小数
            next();
            xasm::content *dest = returnAddress(),src;
            if(dest == nullptr){throw vm_error("first augument must a address or register ");}
            next();
            if(returnAddress() != nullptr) src = *returnAddress();
            else src = this_byte()->c;
            if(check_cmd("_dbl_add",cmd)) dest->dblval() = dest->dblval() + src.dblval();
            if(check_cmd("_dbl_sub",cmd)) dest->dblval() = dest->dblval() - src.dblval();
            if(check_cmd("_dbl_mul",cmd)) dest->dblval() = dest->dblval() * src.dblval();
            if(check_cmd("_dbl_div",cmd)) dest->dblval() = dest->dblval() / src.dblval();
            //if(xasm::cmdset[this_byte()->c.intval()] == "_dbl_mod") *dest = dest->dblval() % src.dblval();
        }else if(check_cmd("eq") || check_cmd("neq") || 
                 check_cmd("maxeq") || check_cmd("mineq") ||
                 check_cmd("max") || check_cmd("min")){
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
            if(check_cmd("eq",cmd)) dest->intval() = src1.intval() == src2.intval();
            if(check_cmd("neq",cmd)) dest->intval() = src1.intval() != src2.intval();
            if(check_cmd("maxeq",cmd)) dest->intval() = src1.intval() >= src2.intval();
            if(check_cmd("mineq",cmd)) dest->intval() = src1.intval() <= src2.intval();
            if(check_cmd("max",cmd)) dest->intval() = src1.intval() > src2.intval();
            if(check_cmd("min",cmd)) dest->intval() = src1.intval() < src2.intval();
        }else if(check_cmd("_itd") || check_cmd("_dti")){
            int cmd = this_byte()->c.intval();
            next();
            xasm::content *dest = returnAddress();
            if(dest == nullptr){throw vm_error("first augument must a address or register");}
            if(check_cmd("_itd",cmd)) dest->dblval() = dest->intval();
            else dest->intval() = dest->dblval();
        }else if(check_cmd("mov1b")){
            next();
            xasm::content *dest = returnAddress(),src;
            if(dest == nullptr){throw vm_error("first augument must a address or register");}
            next();
            if(returnAddress() != nullptr){src = *returnAddress();}
            else src = this_byte()->c;
            next();
            char getbit = this_byte()->c.intval();
            dest->intval() = src.charval()[getbit];
        }else if(check_cmd("push") || check_cmd("pop")){
            int cmd = this_byte()->c.intval();
            next();
            xasm::content dest1,*dest = returnAddress(),size;
            if(dest == nullptr) dest1 = this_byte()->c;
            next();
            if(returnAddress() != nullptr){size = *returnAddress();}
            else size = this_byte()->c;
            if(check_cmd("push",cmd)){
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
        }else if(check_cmd("in") || check_cmd("out")){
            int cmd = this_byte()->c.intval();
            next();
            char* dest = (char*)returnAddress();
            if(dest == nullptr) throw vm_error("expected an address or register");
            next();
            xasm::content num = this_byte()->c;
            if(check_cmd("in",cmd)) port[num.intval()].input(dest);
            else port[num.intval()].output(dest);
        }else if(check_cmd("_virt_fork")){
            next();
            char* dest = (char*)returnAddress();
            if(dest == nullptr || this_byte()->op == xasm::bytecode::_register) throw vm_error("expected an address");
            next();
            memcpy(dest,main_task,sizeof(virt_task));
        }else if(check_cmd("_virt_switch")){
            next();
            char* dest = (char*)returnAddress();
            if(dest == nullptr || this_byte()->op == xasm::bytecode::_register) throw vm_error("expected an address");
            main_task = (virt_task*)dest;
        }else if(check_cmd("_virt_get")){
            next();
            xasm::content* dest = (xasm::content*)returnAddress();
            if(dest == nullptr) throw vm_error("expected an address or register");
            dest->intval() = (long)((char*)main_task - memory) - main_task->base.intval();
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