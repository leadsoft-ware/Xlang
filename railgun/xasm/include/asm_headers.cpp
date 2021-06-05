#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

namespace xasm{
    std::vector<std::string> cmdset = {
        "and","or","add","sub","mul","div","mod", // int command set
        "eq","neq","maxeq","mineq","max","min",
        "_dbl_add","_dbl_mul","_dbl_div","_dbl_mod", // double command set
        "_dbl_eq","_dbl_neq","_dbl_maxeq","_dbl_mineq","_dbl_max","_dbl_min",
        "jmp","call","mov","mov_m","mov1b", // push,pop,save,restore 等由libxasm的内置宏实现 mov1b target,val,pos[0-7]
        "jt","jf", // jt/jf val 如果指定val为真或假，则跳转到相应地方
    };

    // 进行数据操作的基本单位
    struct content{
        double _dblval;
        long long _intval;
        char _charval[8];

        long long &intval(){return _intval;}
        double &dblval(){return _dblval;}
        char* charval(){return (char*)&_charval;}
    };

    bool iskeyword(std::string s){
        if(std::find(cmdset.begin(),cmdset.end(),s) != cmdset.end()) return true;
        else return false;
    }
};