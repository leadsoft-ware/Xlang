/*
 - Xmvef and Xasm support
*/

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>

bool is_number(std::string str){
    for(int i = 0;i < str.length();i++) if(!isdigit(str[i])) return false;
    return true;
}

bool is_decimal(std::string str){
    bool flag = false;
    for(int i = 0;i < str.length();i++){
        if(str[i] == '.' && flag == false){flag = true;continue;}
        if(!isdigit(str[i])){return false;}
    }
    return true;
}

namespace xasm{
    class xasm_error{
        std::string why;
        public:
        xasm_error(std::string str){why = str;}
        std::string what(){
            return "[XtimeVMError][ASM Header APIs] " + why;
        }
    };

    std::vector<std::string> cmdset = {
        "and","or","add","sub","mul","div","mod", // int command set
        "eq","neq","maxeq","mineq","max","min",
        "_dbl_add","_dbl_mul","_dbl_div","_dbl_mod", // double command set
        "_dbl_eq","_dbl_neq","_dbl_maxeq","_dbl_mineq","_dbl_max","_dbl_min",
        "jmp","call","mov","mov_m","mov1b", // push,pop,save,restore 等由libxasm的内置宏实现 mov1b target,val,pos[0-7]
        "jt","jf", // jt/jf val 如果指定val为真或假，则跳转到相应地方
    };

    // 进行数据操作的基本单位
    union content{
        double _dblval;
        long long _intval;
        char _charval[8];

        content(long x){_intval = x;}
        content(long long x){_intval = x;}
        content(int x){_intval = x;}
        content(double d){_dblval = d;}
        long long &intval(){return _intval;}
        double &dblval(){return _dblval;}
        char* charval(){return (char*)&_charval;}
    };

    struct bytecode{
        enum oper{_command,_number,_address,_register,_addr_register} op;
        content c;
    };

    struct bytecode_block{
        std::string block_name;
        std::vector<bytecode> code;
    };

    struct asm_command{
        std::string main;
        std::vector<std::string> args;
    };

    struct asm_block{
        std::string block_name;
        std::vector<asm_command> code;
        asm_block &newCommand(std::string main,std::vector<std::string> args){
            code.push_back((asm_command){main,args});
            return *this;
        }

        // 合并两个块,名字取本块的名字
        asm_block &merge(asm_block &second){
            for(int i = 0;i < second.code.size();i++) code.push_back(second.code[i]);
            return *this;
        }

        // 非引用版
        asm_block &merge(asm_block second){
            for(int i = 0;i < second.code.size();i++) code.push_back(second.code[i]);
            return *this;
        }

        asm_block &setName(std::string name){
            block_name = name;
            return *this;
        }
    };

    // xtime minimal virtual machine executable file -> Xmvef
    struct vmexec_file_header{
        unsigned long long xmvef_sign; // 验证是否为合法文件头 一般为0x114514ff
        unsigned long long from_xlang_package_server; // 是否是官方认证的binary
        char author[32]; // 作者名
        enum distribution_agreement{_gpl,_lgpl,_mit,_wtfpl,_apache} agreement; // 分发所使用的协议，默认gpl
        unsigned long long default_constant_pool_size;
        unsigned long long bytecode_length;
    };

    struct xmvef_file{
        vmexec_file_header head;
        char* constant_pool;
        bytecode* bytecode_array;
    };

    xmvef_file open_xmvef_file(char *path){
        xmvef_file file;
        int fd = open(path,O_RDWR);
        if(fd == -1){throw xasm_error("Cannot open Xmvef file");}
        if(read(fd,&file.head,sizeof(file.head)) == -1){throw xasm_error("Failed to read file header");}
        file.constant_pool = (char*)malloc(file.head.default_constant_pool_size);
        file.bytecode_array = (bytecode*)malloc(file.head.bytecode_length*sizeof(bytecode));
        if(read(fd,&file.constant_pool,sizeof(file.head.default_constant_pool_size)) == -1){throw xasm_error("Failed to read constant pool");}
        if(read(fd,&file.bytecode_array,sizeof(file.head.bytecode_length*sizeof(bytecode))) == -1){throw xasm_error("Failed to read bytecode array");}
        close(fd);
        return file; // 全部文件读取完成
    }

    void create_xmvef_file(char *path,xmvef_file file){
        int fd = open(path,O_RDWR|O_CREAT);
        if(fd == -1){throw xasm_error("Cannot open or create Xmvef file");}
        if(write(fd,&file.head,sizeof(file.head)) == -1){throw xasm_error("Failed to write file header");}
        if(write(fd,&file.constant_pool,sizeof(file.head.default_constant_pool_size)) == -1){throw xasm_error("Failed to write constant pool");}
        if(write(fd,&file.bytecode_array,sizeof(file.head.bytecode_length*sizeof(bytecode))) == -1){throw xasm_error("Failed to write bytecode array");}
        return;
    }

    int iskeyword(std::string s){
        if(std::find(cmdset.begin(),cmdset.end(),s) != cmdset.end()) return std::find(cmdset.begin(),cmdset.end(),s) - cmdset.begin();
        else return -1;
    }
};