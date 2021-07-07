#include <unistd.h>
#include <string>
#include <iostream>
#include "error.cpp"
#include "config.hpp"

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
    TOK_VALUE tok_val;
    std::string str;
    Token(TOK_VALUE tokval,std::string str) : tok_val(tokval) , str(str){ }
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
        if(str[pos] == '\0') {last = Token(tok_eof,"\\0");return Token(tok_eof,"\\0");}
        if(str[pos] == '\n') while(str[pos] == '\n') next();
        if(str[pos] == ' ') while(str[pos] == ' ') next();
        if(isalpha(str[pos]) || str[pos] == '_'){
            int start = pos;
            while( ( isalpha(str[pos]) || isdigit(str[pos]) || str[pos] == '_' ) && str[pos] != '\0' ) next();
            last = Token(tok_id,str.substr(start,pos-start));
            return Token(tok_id,str.substr(start,pos-start));
        }else if(isdigit(str[pos])){
            int start = pos;
            bool f = false;
            while( ( isdigit(str[pos]) || (f == false && (f = str[pos] == '.')) ) && str[pos] != '\0') next();
            last = Token(tok_int,str.substr(start,pos-start));
            return last;
        }else if(str[pos] == '"'){
            next();
            int start = pos;
            while(str[pos] != '"' && str[pos] != '\0') next();
            last = Token(tok_string,str.substr(start,pos-start));
            next();
            return last;
        }else if(str[pos] == '{'){
            next();
            last = Token(tok_mbracketl,"{");
            return Token(tok_mbracketl,"{");
        }else if(str[pos] == '}'){
            next();
            last = Token(tok_mbracketr,"}");
            return Token(tok_mbracketr,"}");
        }else if(str[pos] == '('){
            next();
            last = Token(tok_sbracketl,"(");
            return Token(tok_sbracketl,"(");
        }else if(str[pos] == ')'){
            next();
            last = Token(tok_sbracketr,")");
            return Token(tok_sbracketr,")");
        }else if(str[pos] == '['){
            next();
            last = Token(tok_cbracketl,"[");
            return Token(tok_cbracketl,"[");
        }else if(str[pos] == ']'){
            next();
            last = Token(tok_cbracketr,"]");
            return Token(tok_cbracketr,"]");
        }else if(str[pos] == ','){
            next();
            last = Token(tok_comma,",");
            return Token(tok_comma,",");
        }else if(str[pos] == '+'){
            next();
            if(str[pos] == '='){next();last = Token(tok_addwith,"+=");return Token(tok_addwith,"+=");}
            else if(str[pos] == '+'){next();last = Token(tok_addself,"++");return Token(tok_addself,"++");}
            last = Token(tok_add,"+");
            return Token(tok_add,"+");
        }else if(str[pos] == '-'){
            next();
            if(str[pos] == '='){next();last = Token(tok_subwith,"-=");return last;}
            else if(str[pos] == '-'){next();last = Token(tok_subself,"--");return last;}
            else if(str[pos] == '>'){next();last = Token(tok_hrefto,"->");return last;}
            last = Token(tok_sub,"-");
            return last;
        }else if(str[pos] == '*'){
            next();
            if(str[pos] == '='){next();last = Token(tok_mulwith,"*=");return last;}
            last = Token(tok_mul,"*");
            return last;
        }else if(str[pos] == '/'){
            next();
            if(str[pos] == '/'){
                while(str[pos] != '\n') next();
                return getNextToken();
            }
            if(str[pos] == '*'){
                while(str[pos-1] != '*' && str[pos] != '/') next();
                return getNextToken();
            }
            if(str[pos] == '='){next();last = Token(tok_divwith,"/=");return last;}
            last = Token(tok_div,"/");
            return last;
        }else if(str[pos] == '%'){
            next();
            if(str[pos] == '%'){next();last = Token(tok_modwith,"%=");return last;}
            last = Token(tok_mod,"%");
            return last;
        }else if(str[pos] == '='){
            next();
            if(str[pos] == '='){next();last = Token(tok_equal,"==");return last;}
            last = Token(tok_eq,"=");
            return last;
        }else if(str[pos] == '>'){
            next();
            if(str[pos] == '='){next();last = Token(tok_maxeq,">=");return last;}
            last = Token(tok_max,">");
            return last;
        }else if(str[pos] == '<'){
            next();
            if(str[pos] == '='){next();last = Token(tok_mineq,"<=");return last;}
            last = Token(tok_min,"<");
            return last;
        }else if(str[pos] == '&'){
            next();
            if(str[pos] == '&'){next();last = Token(tok_and,"&&");return last;}
            throw compiler_error("Unexpected Token:" + std::string(""+str[pos]) + "\nIn xlang, binary expression doesn't work with this form",line+1,col+1);
        }else if(str[pos] == '|'){
            next();
            if(str[pos] == '|'){next();last = Token(tok_or,"||");return last;}
            throw compiler_error("Unexpected Token:" + std::string(""+str[pos]) + "\nIn xlang, binary expression doesn't work with this form",line+1,col+1);
        }else if(str[pos] == '.'){
            next();
            last = Token(tok_dot,".");
            return last;
        }else if(str[pos] == ';'){
            next();
            last = Token(tok_semicolon,";");
            return last;
        }else if(str[pos] == ':'){
            next();
            last = Token(tok_colon,":");
            return last;
        }else{
            throw compiler_error(std::string("Unexpected Token") + str[pos],line,col);
        }
    }
    Lexer(std::string str) : str(str){
        line = col = pos = 0;
    }
};