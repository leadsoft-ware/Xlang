#include <string>
#include <iostream>

class compiler_error : public std::exception{
    std::string error_kind;
    int line,col;
    public:
    compiler_error(std::string err,int line,int col){
        this->error_kind = err;
        this->line = line;
        this->col = col;
    }
    void what(){
        std::cerr << "[XlangError] At line " << line+1 << " col " << col+1 << " " << error_kind << std::endl;
    }
};