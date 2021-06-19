#include "../xasm/include/xasm.cpp"

void dump(char* filename){
    xasm::xmvef_file xmvef = xasm::open_xmvef_file(filename);
    if(xmvef.head.xmvef_sign != 0x114514ff) return;
    std::cout << "[Xmvef File dump] filename:" << filename << "\n";
    std::cout << "bytecode length:" << xmvef.head.bytecode_length << " database size:" << xmvef.head.default_constant_pool_size << " pc_start:" << xmvef.head.start_of_pc << "\n";
    std::cout << "disasm result:\n";
    for(int i = 0;i < xmvef.head.bytecode_length;i++){
        if(xmvef.bytecode_array[i].op == xasm::bytecode::_command){ std::cout << xasm::cmdset[xmvef.bytecode_array[i].c.intval()] << "(";continue;}
        else if(xmvef.bytecode_array[i].op == xasm::bytecode::_number) std::cout << xmvef.bytecode_array[i].c.intval();
        else if(xmvef.bytecode_array[i].op == xasm::bytecode::_address) std::cout << "*" << xmvef.bytecode_array[i].c.intval();
        else if(xmvef.bytecode_array[i].op == xasm::bytecode::_register) std::cout << "reg" << xmvef.bytecode_array[i].c.intval();
        else if(xmvef.bytecode_array[i].op == xasm::bytecode::_addr_register) std::cout << "*reg" << xmvef.bytecode_array[i].c.intval();

        if(i != xmvef.head.bytecode_length - 1 && xmvef.bytecode_array[i+1].op != xasm::bytecode::_command) std::cout << ",";
        else if(i != xmvef.head.bytecode_length - 1 && xmvef.bytecode_array[i+1].op == xasm::bytecode::_command) std::cout << ");\n";
        else if(i == xmvef.head.bytecode_length - 1) std::cout << ");\n";
    }
    std::cout << "disasm complete.\n";
}

int main(int argc,const char** argv){
    dump((char*)argv[1]);
    return 0;
}