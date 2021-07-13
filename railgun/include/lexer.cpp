#include <unistd.h>
#include <string>
#include <iostream>
#include "error.cpp"
#include "config.hpp"
#include <stdlib.h>
#include <ctype.h>


enum TOK_VALUE{
    tok_id,tok_charter,tok_string,tok_int, /*id,'any',"any",num[.num]*/
    tok_add,tok_sub,tok_mul,tok_div,tok_mod,/*+,-,*,/,%*/
    tok_eq,tok_equal,tok_noteq,tok_maxeq,tok_mineq,tok_max,tok_min,tok_and,tok_or, /* = == != >= <= > < && || */
    tok_addself,tok_subself,/*++,--*/
    tok_addwith,tok_subwith,tok_mulwith,tok_divwith,tok_modwith,/* += -= *= /= %= */
    tok_semicolon,tok_comma,tok_cbracketl,tok_cbracketr,tok_sbracketl,tok_sbracketr,tok_mbracketl,tok_mbracketr, /* ; , () [] {}*/
    tok_colon,tok_dot,tok_hrefto,tok_eof, /* . -> \0 */
};

std::string TOK_DESP[] = {
    "tok_id","tok_charter","tok_string","tok_int", /*id,'any',"any",num[.num]*/
    "tok_add","tok_sub","tok_mul","tok_div","tok_mod",/*+,-,*,/,%*/
    "tok_eq","tok_equal","tok_noteq","tok_maxeq","tok_mineq","tok_max","tok_min","tok_and","tok_or", /* = == != >= <= > < && ||*/
    "tok_addself","tok_subself",/*++,--*/
    "tok_addwith","tok_subwith","tok_mulwith","tok_divwith","tok_modwith",/* += -= *= /= %= */
    "tok_semicolon","tok_comma","tok_cbracketl","tok_cbracketr","tok_sbracketl","tok_sbracketr","tok_mbracketl","tok_mbracketr", /* ; , () [] {}*/
    "tok_colon","tok_dot","tok_hrefto","tok_eof",
};

struct Token{
    size_t line,col;
    TOK_VALUE tok_val;
    std::string str;
    Token(TOK_VALUE tokval,std::string str,size_t line,size_t col) : tok_val(tokval) , str(str) , line(line) , col(col){ }
    Token(){} // for class init
    std::string toString(){return TOK_DESP[tok_val]+":"+str;}
};

class Lexer{
    std::string str;
    public:
    Token last;
    int line,col,pos;
    void next(){
        pos++;
        if(str[pos-1] == '\0'){pos--;return;}
        if(str[pos] == '\0'){return;}
        if(str[pos] == '\n'){line++;col = 0;}
        else{col++;}
        return;
    }
    Token getNextToken(){
        if(str[pos] == '\0') {last = Token(tok_eof,"\\0",this->line,this->col);return last;}
        if(str[pos] == '\n') while(str[pos] == '\n') next();
        if(str[pos] == ' ') while(str[pos] == ' ') next();
        if(isalpha(str[pos]) || str[pos] == '_'){
            int start = pos;
            while( ( isalpha(str[pos]) || isdigit(str[pos]) || str[pos] == '_' ) && str[pos] != '\0' ) next();
            last = Token(tok_id,str.substr(start,pos-start),this->line,this->col);
            return last;
        }else if(isdigit(str[pos])){
            int start = pos;
            bool f = false;
            while( ( isdigit(str[pos]) || (f == false && (f = str[pos] == '.')) ) && str[pos] != '\0') next();
            last = Token(tok_int,str.substr(start,pos-start),this->line,this->col);
            return last;
        }else if(str[pos] == '"'){
            next();
            int start = pos;
            while(str[pos] != '"' && str[pos] != '\0') next();
            last = Token(tok_string,str.substr(start,pos-start),this->line,this->col);
            next();
            return last;
        }else if(str[pos] == '{'){
            next();
            last = Token(tok_mbracketl,"{",this->line,this->col);
            return last;
        }else if(str[pos] == '}'){
            next();
            last = Token(tok_mbracketr,"}",this->line,this->col);
            return last;
        }else if(str[pos] == '('){
            next();
            last = Token(tok_sbracketl,"(",this->line,this->col);
            return last;
        }else if(str[pos] == ')'){
            next();
            last = Token(tok_sbracketr,")",this->line,this->col);
            return last;
        }else if(str[pos] == '['){
            next();
            last = Token(tok_cbracketl,"[",this->line,this->col);
            return last;
        }else if(str[pos] == ']'){
            next();
            last = Token(tok_cbracketr,"]",this->line,this->col);
            return last;
        }else if(str[pos] == ','){
            next();
            last = Token(tok_comma,",",this->line,this->col);
            return last;
        }else if(str[pos] == '+'){
            next();
            if(str[pos] == '='){next();last = Token(tok_addwith,"+=",this->line,this->col);return last;}
            else if(str[pos] == '+'){next();last = Token(tok_addself,"++",this->line,this->col);return last;}
            last = Token(tok_add,"+",this->line,this->col);
            return last;
        }else if(str[pos] == '-'){
            next();
            if(str[pos] == '='){next();last = Token(tok_subwith,"-=",this->line,this->col);return last;}
            else if(str[pos] == '-'){next();last = Token(tok_subself,"--",this->line,this->col);return last;}
            else if(str[pos] == '>'){next();last = Token(tok_hrefto,"->",this->line,this->col);return last;}
            last = Token(tok_sub,"-",this->line,this->col);
            return last;
        }else if(str[pos] == '*'){
            next();
            if(str[pos] == '='){next();last = Token(tok_mulwith,"*=",this->line,this->col);return last;}
            last = Token(tok_mul,"*",this->line,this->col);
            return last;
        }else if(str[pos] == '/'){
            next();
            if(str[pos] == '/'){
                while(str[pos] != '\n') next();
                return getNextToken();
            }
            if(str[pos] == '*'){
                while(str[pos-1] != '*' && str[pos] != '/') next(); // FIXME
                return getNextToken();
            }
            if(str[pos] == '='){next();last = Token(tok_divwith,"/=",this->line,this->col);return last;}
            last = Token(tok_div,"/",this->line,this->col);
            return last;
        }else if(str[pos] == '%'){
            next();
            if(str[pos] == '='){next();last = Token(tok_modwith,"%=",this->line,this->col);return last;}
            last = Token(tok_mod,"%",this->line,this->col);
            return last;
        }else if(str[pos] == '='){
            next();
            if(str[pos] == '='){next();last = Token(tok_equal,"==",this->line,this->col);return last;}
            last = Token(tok_eq,"=",this->line,this->col);
            return last;
        }else if(str[pos] == '>'){
            next();
            if(str[pos] == '='){next();last = Token(tok_maxeq,">=",this->line,this->col);return last;}
            last = Token(tok_max,">",this->line,this->col);
            return last;
        }else if(str[pos] == '<'){
            next();
            if(str[pos] == '='){next();last = Token(tok_mineq,"<=",this->line,this->col);return last;}
            last = Token(tok_min,"<",this->line,this->col);
            return last;
        }else if(str[pos] == '&'){
            next();
            if(str[pos] == '&'){next();last = Token(tok_and,"&&",this->line,this->col);return last;}
            throw compiler_error("Unexpected Token:" + std::string(""+str[pos]) + "\nIn xlang, binary expression doesn't work with this form",line+1,col+1);
        }else if(str[pos] == '|'){
            next();
            if(str[pos] == '|'){next();last = Token(tok_or,"||",this->line,this->col);return last;}
            throw compiler_error("Unexpected Token:" + std::string(""+str[pos]) + "\nIn xlang, binary expression doesn't work with this form",line+1,col+1);
        }else if(str[pos] == '.'){
            next();
            last = Token(tok_dot,".",this->line,this->col);
            return last;
        }else if(str[pos] == ';'){
            next();
            last = Token(tok_semicolon,";",this->line,this->col);
            return last;
        }else if(str[pos] == ':'){
            next();
            last = Token(tok_colon,":",this->line,this->col);
            return last;
        }else{
            throw compiler_error(std::string("Unexpected Token") + str[pos],line,col);
        }
    }
    Lexer(std::string str) : str(str){
        line = col = pos = 0;
    }
};