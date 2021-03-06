#include <string>
#include <sstream>
#include <iostream>

class compiler_error{
    std::string error_kind;
    int line,col;
    public:
    compiler_error(std::string err,int line,int col){
        this->error_kind = err;
        this->line = line;
        this->col = col;
    }
    std::string what(){
        std::stringstream ss;
        ss << "[XlangError] At line " << line+1 << " col " << col+1 << " " << error_kind << std::endl;
        return ss.str();
    }
};

void sendWarning(std::string msg,int line,int col){
    std::cout << "[XlangWarning] At line " << line+1 << " col " << col+1 << " " << msg << std::endl;
}

void sendLog(std::string level,std::string msg){
    std::cout << "[XlangLog/" << level << "]" << msg << std::endl;
}