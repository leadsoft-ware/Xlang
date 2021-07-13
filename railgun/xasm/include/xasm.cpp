#pragma once

#include "../../include/xast.cpp"
#include "asm_headers.cpp"
#include <map>

namespace xasm{
    std::map<std::string,xast::astree> marcos; // for user marcos
    std::string database; // 常量数据池
};

namespace xast::rule_parser{
    class asm_primary_parser{
        Lexer *lexer;
        public:
        asm_primary_parser(Lexer *lexer){this->lexer = lexer;}
        xast::astree match(){
            backup_for_rollback;
            if(lexer->last.tok_val == tok_mul){
                lexer->getNextToken();
                xast::astree primary = xast::rule_parser::primary_parser(lexer).match();
                if(primary.matchWithRule == ""){failed_to_match;}
                primary.matchWithRule = "asm_primary_pointer";
                return primary;
            }
            xast::astree primary = xast::rule_parser::primary_parser(lexer).match();
            if(primary.matchWithRule != "") primary.matchWithRule = "asm_primary";
            return primary;
        }
    };

    class asm_argument_parser{
        Lexer *lexer;
        public:
        asm_argument_parser(Lexer *lexer){this->lexer = lexer;}
        xast::astree match(){
            xast::astree ast;
            ast.t = xast::astree::leaf;
            ast.matchWithRule = "argument";
            if(lexer->last.tok_val == tok_sbracketr || lexer->last.tok_val == tok_semicolon){return ast;} // 没有参数，返回，小括号右端要匹配，不用跳过
            ast.node.push_back(xast::rule_parser::asm_primary_parser(lexer).match());
            while(lexer->last.tok_val == tok_comma){
                lexer->getNextToken();
                xast::astree newer = xast::rule_parser::asm_primary_parser(lexer).match();
                ast.node.push_back(newer);
            }
            return ast;
        }
    };

    class asm_db_stmt_parser{
        Lexer *lexer;
        public:
        asm_db_stmt_parser(Lexer *lexer){this->lexer = lexer;}
        xast::astree match(){
            backup_for_rollback;
            if(lexer->last.str != "_db"){failed_to_match;}
            lexer->getNextToken();
            if(lexer->last.tok_val != tok_sbracketl){throw compiler_error("expected a '(' befoce arguments.",lexer->line,lexer->col);} // 与上注释相同
            lexer->getNextToken();
            if(lexer->last.tok_val != tok_int){throw compiler_error("expected a data address",lexer->line,lexer->col);} // 与上注释相同
            xast::astree root("asm_db_stmt",{xast::astree("primary",lexer->last)});
            lexer->getNextToken();
            if(lexer->last.tok_val != tok_comma){throw compiler_error("expected a comma befoce asm block",lexer->line,lexer->col);} // 与上注释相同
            lexer->getNextToken();
            if(lexer->last.tok_val != tok_string && lexer->last.tok_val != tok_int){throw compiler_error("expected a primary token",lexer->line,lexer->col);} // 与上注释相同
            root.node.push_back({xast::astree("primary",lexer->last)});
            lexer->getNextToken();
            return root;
        }
    };

    class asm_stmt_parser{
        Lexer *lexer;
        public:
        asm_stmt_parser(Lexer *lexer){this->lexer = lexer;}
        // 匹配一行ast语句
        xast::astree match(){
            backup_for_rollback;
            xast::astree root;
            if( lexer->last.tok_val != tok_id || ( xasm::iskeyword(lexer->last.str) == -1 && !xasm::marcos.count(lexer->last.str) ) ){failed_to_match;}
            if(xasm::iskeyword(lexer->last.str) != -1) root = xast::astree("asm_"+lexer->last.str,Token());
            else root = xast::astree("marco_"+lexer->last.str,Token());

            lexer->getNextToken();
            if(lexer->last.tok_val != tok_sbracketl){throw compiler_error("expected an '(' befoce arguments",lexer->line,lexer->col);}
            lexer->getNextToken();
            xast::astree args = xast::rule_parser::asm_argument_parser(lexer).match();
            if(lexer->last.tok_val != tok_sbracketr){throw compiler_error("expected an ')' after arguments",lexer->line,lexer->col);}
            lexer->getNextToken();

            if(args.matchWithRule == ""){root.t = xast::astree::child;return root;}
            root.node.push_back(args);
            return root;
        }
    };
    class asm_block_parser{
        Lexer *lexer;
        public:
        asm_block_parser(Lexer* lexer){this->lexer = lexer;}
        xast::astree match(){
            if(lexer->last.tok_val != tok_mbracketl){throw compiler_error("expected a '{' befoce block.",lexer->line,lexer->col);} // 与上注释相同
            lexer->getNextToken();
            xast::astree root("_asm_block",Token());
            xast::astree temp = xast::rule_parser::asm_stmt_parser(lexer).match();
            if(temp.matchWithRule == ""){lexer->getNextToken();return root;}
            while(temp.matchWithRule != ""){
                root.node.push_back(temp);
                if(lexer->last.tok_val != tok_semicolon){throw compiler_error("expected a semicolon before next statement.",lexer->line,lexer->col);} // 与上注释相同
                lexer->getNextToken();
                temp = xast::rule_parser::asm_stmt_parser(lexer).match();
            }
            //lexer->getNextToken();
            if(lexer->last.tok_val != tok_mbracketr){throw compiler_error("expected a '}' after block.",lexer->line,lexer->col);} // 与上注释相同
            //lexer->getNextToken();
            return root;
        }
    };
    class asm_block_stmt_parser{
        Lexer *lexer;
        public:
        asm_block_stmt_parser(Lexer* lexer){this->lexer = lexer;}
        xast::astree match(){
            xast::astree primary = xast::rule_parser::primary_parser(lexer).match();
            xast::astree root("asmblock",{primary});
            if(primary.matchWithRule == "" || lexer->last.tok_val != tok_colon){throw compiler_error("asm block must have a name.",lexer->line,lexer->col);} // 与上注释相同
            lexer->getNextToken();
            xast::astree block = xast::rule_parser::asm_block_parser(lexer).match();
            root.node.push_back(block);
            return root;
        }
    };
    class asm_marco_stmt_parser{
        Lexer *lexer;
        public:
        asm_marco_stmt_parser(Lexer *lexer){this->lexer = lexer;}
        xast::astree match(){
            backup_for_rollback;
            if(lexer->last.str != "set_marco"){failed_to_match;}
            lexer->getNextToken();
            if(lexer->last.tok_val != tok_sbracketl){throw compiler_error("expected a '(' befoce arguments.",lexer->line,lexer->col);} // 与上注释相同
            lexer->getNextToken();
            if(lexer->last.tok_val != tok_id){throw compiler_error("expected a marco name",lexer->line,lexer->col);} // 与上注释相同
            xast::astree root("asm_marco_stmt",{xast::astree("primary",lexer->last)});
            lexer->getNextToken();
            if(lexer->last.tok_val != tok_comma){throw compiler_error("expected a comma befoce asm block",lexer->line,lexer->col);} // 与上注释相同
            lexer->getNextToken();
            xast::astree block = xast::rule_parser::asm_block_parser(lexer).match();
            lexer->getNextToken();
            root.node.push_back(block);
            return root;
        }
    };

    class asm_db_size_stmt_parser{
        Lexer *lexer;
        public:
        asm_db_size_stmt_parser(Lexer *lexer){this->lexer = lexer;}
        xast::astree match(){
            backup_for_rollback;
            if(lexer->last.str != "_set_db_size"){failed_to_match;}
            lexer->getNextToken();
            if(lexer->last.tok_val != tok_sbracketl){throw compiler_error("expected a '(' befoce arguments.",lexer->line,lexer->col);} // 与上注释相同
            lexer->getNextToken();
            if(lexer->last.tok_val != tok_int){throw compiler_error("expected a database size",lexer->line,lexer->col);} // 与上注释相同
            xast::astree root("asm_db_size_stmt",{xast::astree("primary",lexer->last)});
            lexer->getNextToken();
            return root;
        }
    };

    class asm_main_stmt_parser{
        Lexer *lexer;
        public:
        asm_main_stmt_parser(Lexer *lexer){this->lexer = lexer;}
        xast::astree match(){
            xast::astree ret;
            ret = xast::rule_parser::import_stmt_parser(lexer).match();
            if(ret.matchWithRule != "") return ret;
            ret = xast::rule_parser::asm_db_size_stmt_parser(lexer).match();
            if(ret.matchWithRule != "") return ret;
            ret = xast::rule_parser::asm_db_stmt_parser(lexer).match();
            if(ret.matchWithRule != "") return ret;
            ret = xast::rule_parser::asm_marco_stmt_parser(lexer).match();
            if(ret.matchWithRule != "") return ret;
            ret = xast::rule_parser::asm_block_stmt_parser(lexer).match();
            return ret;
        }
    };
};

namespace xasm{
    // 翻译
    asm_block translateToASMStruct(xast::astree stmt){
        asm_block ret;
        if(stmt.matchWithRule == "asmblock"){
            ret.block_name = stmt.node[0].tok.str;
            for(int i = 0;i < stmt.node[1].node.size();i++) ret.merge(translateToASMStruct(stmt.node[1].node[i]));
            return ret;
        }else if(stmt.matchWithRule == "_asm_block"){
            for(int i = 0;i < stmt.node.size();i++) ret.merge(translateToASMStruct(stmt.node[i]));
            return ret;
        }else if(stmt.matchWithRule.substr(0,4) == "asm_"){
            std::vector<std::string> args;
            for(int i = 0;i < stmt.node[0].node.size();i++){
                if(stmt.node[0].node[i].matchWithRule != "asm_primary" && stmt.node[0].node[i].matchWithRule != "asm_primary_pointer"){throw compiler_error("expected an primary token.",stmt.node[0].node[i].tok.line,stmt.node[0].node[i].tok.col);} // 与上注释相同
                if(stmt.node[0].node[i].matchWithRule == "asm_primary") args.push_back(stmt.node[0].node[i].tok.str);
                else args.push_back("_addr_" + stmt.node[0].node[i].tok.str);
            }
            asm_command c = (asm_command){stmt.matchWithRule.substr(4),args};
            ret.code.push_back(c);
            return ret;
        }else if(stmt.matchWithRule.substr(0,6) == "marco_"){
            // quick setup: mei you bian li hong de can shu
            ret.merge(translateToASMStruct(marcos[stmt.matchWithRule.substr(6)]));
            // 遍历每一行语句
            for(int i = 0;i < ret.code.size();i++){
                // 遍历每一个宏的调用参数
                for(int j = 0;j < stmt.node[0].node.size();j++){
                    if(ret.code[i].main == "_marco_param" + std::to_string(j)) ret.code[i].main = stmt.node[0].node[j].tok.str;
                    // 遍历每一行语句的参数
                    for(int k = 0;k < ret.code[i].args.size();k++){
                        //std::cout << ret.code[i].args[k] << " " << "_marco_param" << std::to_string(j) << std::endl;
                        if(ret.code[i].args[k] != "_marco_param" + std::to_string(j)) continue;
                        if(stmt.node[0].node[j].matchWithRule == "asm_primary_pointer") ret.code[i].args[k] = "_addr_" + stmt.node[0].node[j].tok.str;
                        else ret.code[i].args[k] = stmt.node[0].node[j].tok.str;
                    }
                }
            }
            return ret;
        }else{
            throw compiler_error("unknown asm match rule.",stmt.tok.line,stmt.tok.col);
        }
    }

    //  计算从指定块开始到另一个指定块的距离
    long long  countBytecodeBlock(std::vector<xasm::bytecode_block> &block_map,int idx_of_start = 0,int idx_of_end = INT32_MAX){
        long long  ret = 0;
        if(idx_of_start == idx_of_end) return 0;
        for(int i = idx_of_start;i != block_map.size();i++){
            if(i == idx_of_end) return ret;
            ret += block_map[i].code.size();
        }
        return ret;
    }

    // return an index of block
    int inBlockMap(std::string name,std::vector<xasm::bytecode_block> &block_map){
        for(auto i = block_map.begin();i != block_map.end();i++){
            if(i->block_name == name) return i - block_map.begin();
        }
        return -1;
    }

    bytecode_block translateToBytecode(asm_block &a,std::vector<xasm::bytecode_block> &block_map){
        bytecode_block ret;
        ret.block_name = a.block_name;
        for(int i = 0;i < a.code.size();i++){
            ret.code.push_back((bytecode){bytecode::_command,iskeyword(a.code[i].main)});
            for(int j = 0;j < a.code[i].args.size();j++){
                std::string &str = a.code[i].args[j];
                // 判断是否为函数名
                if(inBlockMap(str,block_map) != -1){
                    std::cout << str;
                    std::cout << ":" << inBlockMap(str,block_map) << " " << xasm::database.size() << std::endl;
                    ret.code.push_back((bytecode){bytecode::_number,(long long) (xasm::database.size() + countBytecodeBlock(block_map,0, inBlockMap(str,block_map)) * sizeof(xasm::bytecode) )}); // 计算块的位置并加入字节码, 加一是因为计算的是块离0的距离，还得+1
                    continue;
                }

                if(str.substr(0,6) == "_addr_"){
                    if(str.substr(6).substr(0,3) == "reg") ret.code.push_back((bytecode){bytecode::_addr_register,stol(str.substr(6).substr(3))});
                    else if(is_number(str.substr(6))) ret.code.push_back((bytecode){bytecode::_address,stol(str.substr(6))});
                    else throw compiler_error("[unknown position] unexecpted address",0,0);
                    continue;
                }

                if(str.substr(0,3) == "reg"){
                    ret.code.push_back((bytecode){bytecode::_register,stol(str.substr(3))});
                }
                else if(is_decimal(str)) ret.code.push_back((bytecode){bytecode::_number,stod(str)});
                else if(is_number(str)) {
                    ret.code.push_back((bytecode){bytecode::_number,stol(str)});
                }
                else throw compiler_error("[unknown position] unknown argument syntax",0,0);
            }
        }
        return ret;
    }
};