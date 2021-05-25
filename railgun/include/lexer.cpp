#include <unistd.h>
#include <string>
#include <iostream>
#include "error.cpp"

enum TOK_VALUE{
    tok_id,tok_charter,tok_string,tok_int, /*id,'any',"any",num[.num]*/
    tok_add,tok_sub,tok_mul,tok_div,tok_mod,/*+,-,*,/,%*/
    tok_addself,tok_subself,/*++,--*/
    tok_addwith,tok_subwith,tok_mulwith,tok_divwith,tok_modwith,/* += -= *= /= %= */
    tok_semicolon,tok_colon,tok_cbracket,tok_sbracket,tok_block, /* ; , () [] {}*/
    tok_ptr,tok_getaddr,tok_eof,
};

std::string TOK_DESP[] = {
    "tok_id","tok_charter","tok_string","tok_int", /*id,'any',"any",num[.num]*/
    "tok_add","tok_sub","tok_mul","tok_div","tok_mod",/*+,-,*,/,%*/
    "tok_addself","tok_subself",/*++,--*/
    "tok_addwith","tok_subwith","tok_mulwith","tok_divwith","tok_modwith",/* += -= *= /= %= */
    "tok_semicolon","tok_colon","tok_cbracket","tok_sbracket","tok_block", /* ; , () [] {}*/
    "tok_ptr","tok_getaddr","tok_eof",
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
        if(str[pos] == '\0'){return;}
        if(str[pos] == '\n'){line++;}
        else{col++;}
        return;
    }
    Token getNextToken(){
        if(str[pos] == '\0') return Token(tok_eof,"\\0");
        if(str[pos] == '\n') while(str[pos] == '\n') next();
        if(str[pos] == ' ') while(str[pos] == ' ') next();
        if(isalpha(str[pos]) || str[pos] == '_'){
            int start = pos;
            while( ( isalpha(str[pos]) || isdigit(str[pos]) || str[pos] == '_' ) && str[pos] != '\0' ) next();
            return Token(tok_id,str.substr(start,pos-start));
        }else if(isdigit(str[pos])){
            int start = pos;
            bool f = false;
            while( ( isdigit(str[pos]) || (f == false && (f = str[pos] == '.')) ) && str[pos] != '\0') next();
            return Token(tok_int,str.substr(start,pos-start));
        }else if(str[pos] == '{'){
            int start = pos;
            int looper = 1;
            next();
            while(looper){
                if(str[pos] == '{') looper ++;
                if(str[pos] == '}') looper --;
                next();
            }
            return Token(tok_block,str.substr(start,pos-start));
        }else if(str[pos] == '('){
            int start = pos;
            int looper = 1;
            next();
            while(looper){
                if(str[pos] == '(') looper ++;
                if(str[pos] == ')') looper --;
                next();
            }
            return Token(tok_cbracket,str.substr(start,pos-start));
        }else if(str[pos] == '['){
            int start = pos;
            int looper = 1;
            next();
            while(looper){
                if(str[pos] == '[') looper ++;
                if(str[pos] == ']') looper --;
                next();
            }
            return Token(tok_sbracket,str.substr(start,pos-start));
        }else if(str[pos] == ','){
            next();
            return Token(tok_colon,",");
        }else if(str[pos] == '+'){
            next();
            if(str[pos] == '='){next();return Token(tok_addwith,"+=");}
            else if(str[pos] == '+'){next();return Token(tok_addself,"++");}
            return Token(tok_add,"+");
        }else if(str[pos] == '-'){
            next();
            if(str[pos] == '='){next();return Token(tok_subwith,"-=");}
            else if(str[pos] == '-'){next();return Token(tok_subself,"--");}
            return Token(tok_add,"-");
        }else if(str[pos] == '*'){
            next();
            if(str[pos] == '='){next();return Token(tok_mulwith,"*=");}
            else if(str[pos] == '('){return Token(tok_ptr,getNextToken().str);} // 特殊情况特殊处理
            return Token(tok_mul,"*");
        }else if(str[pos] == '/'){
            next();
            if(str[pos] == '='){next();return Token(tok_divwith,"/=");}
            return Token(tok_div,"/");
        }else if(str[pos] == '%'){
            next();
            if(str[pos] == '%'){next();return Token(tok_modwith,"%=");}
            return Token(tok_mod,"%");
        }else if(str[pos] == '&'){
            next();
            if(str[pos] == '('){return Token(tok_getaddr,getNextToken().str);} // 特殊情况特殊处理
            throw compiler_error("Unexecpted Token" + str[pos],line,col);
        }else{
            throw compiler_error("Unexecpted Token" + str[pos],line,col);
        }
    }
    Lexer(std::string str){
        this->str = str;
        line = col = pos = 0;
    }
};