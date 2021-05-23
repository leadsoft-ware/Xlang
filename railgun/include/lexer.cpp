#include <unistd.h>
#include <string>
#include <iostream>
#include "error.cpp"

enum TOK_VALUE{
    tok_id,tok_charter,tok_string,tok_int, /*id,'any',"any",num[.num]*/
    tok_add,tok_sub,tok_mul,tok_div,tok_mod,/*+,-,*,/,%*/
    tok_addself,tok_subself,/*++,--*/
    tok_addwith,tok_subwith,tok_mulwith,tok_divwith,tok_modwith,/* += -= *= /= %= */
    tok_semicolon,tok_colon,tok_cbracket,tok_sbracket,tok_code, /* ; , () [] {}*/
    tok_ptr,tok_ptrb,tok_eof,
};

std::string TOK_DESP[] = {
    "tok_id","tok_charter","tok_string","tok_int", /*id,'any',"any",num[.num]*/
    "tok_add","tok_sub","tok_mul","tok_div","tok_mod",/*+,-,*,/,%*/
    "tok_addself","tok_subself",/*++,--*/
    "tok_addwith","tok_subwith","tok_mulwith","tok_divwith","tok_modwith",/* += -= *= /= %= */
    "tok_semicolon","tok_colon","tok_cbracket","tok_sbracket","tok_code", /* ; , () [] {}*/
    "tok_ptr","tok_ptrb","tok_eof",
};

struct Token{
    TOK_VALUE tok_val;
    std::string str;
    Token(TOK_VALUE tokval,std::string str){this->tok_val = tokval;this->str = str;}
    std::string toString(){return TOK_DESP[tok_val]+":"+str;}
};

class Lexer{
    int line,col,pos;
    std::string str;
    public:
    void next(){
        pos++;
        if(str[pos] == '\0'){pos--;return;}
        if(str[pos] == '\n'){line++;}
        else{col++;}
        return;
    }
    Token getNextToken(){
        if(str[pos] == '\0') return Token(tok_eof,"\\0");
        if(str[pos] == '\n') while(str[pos] == '\n') next();
        if(isalpha(str[pos]) || str[pos] == '_'){
            int start = pos;
            while( ( isalpha(str[pos]) || isdigit(str[pos]) || str[pos] == '_' ) && str[pos] != '\0' ) next();
            return Token(tok_id,str.substr(start,pos-start));
        }else if(isdigit(str[pos])){
            int start = pos;
            bool f = false;
            while( ( isdigit(str[pos]) || (f == false && (f = str[pos] == '.')) ) && str[pos] != '\0') next();
            return Token(tok_int,str.substr(start,pos-start));
        }else{
            throw compiler_error("Unexecpted Token" + str[pos],line,col);
        }
    }
    Lexer(std::string str){
        this->str = str;
        line = col = pos = 0;
    }
};