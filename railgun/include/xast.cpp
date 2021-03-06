/*
    Description:
    xast实现
*/
#pragma once

#include "xast.hpp"

#define backup_for_rollback int line = lexer->line,col = lexer->col,pos = lexer->pos; Token backup_token = lexer->last
#define reset_to_backup lexer->line = line,lexer->col = col,lexer->pos = pos;lexer->last = backup_token
#define failed_to_match reset_to_backup; return astree()

std::string getSpace(int count){
    std::string ret;
    while(count--) ret += ' ';
    return ret;
}

xast::astree::astree(){
    matchWithRule = "";
    this->t = _child;
}

// init for _child
xast::astree::astree(std::string matchWithRule,Token t){
    this->t = _child;
    this->matchWithRule = matchWithRule;
    if(t.str == "") this->t = _node; // 偷懒
    this->tok = t;
}

xast::astree::astree(std::string matchWithRule,std::vector<astree> n){
    this->t = _node;
    this->matchWithRule = matchWithRule;
    this->node = n;
}

std::string xast::astree::toString(int deepth){
    std::stringstream ss;
    // 在递归之前必须开新行
    ss << "{" << std::endl << getSpace(deepth); // (lastline) (spaces: this_deepth - 1) { (nextline) (spaces: this_deepth)
    ss << "type: " << "\"" << ( (this->t == _node) ? "node" : "_child" ) << "\"," << std::endl << getSpace(deepth); // type: "any", (nextline) (space: thisdeepth)
    ss << "match: " << "\"" << this->matchWithRule << "\"," << std::endl; // type: "any", (nextline) (space: thisdeepth)
    if(this->t == _node){
        ss << getSpace(deepth) << "node: [\n";
        for(int i = 0;i < this->node.size();i++){
            ss << getSpace(deepth+1) << this->node[i].toString(deepth+2) << ",";
        }
        ss << "\b\n" << getSpace(deepth) << "]";
    }else{
        ss << getSpace(deepth) << "token: " << "\"" << this->tok.toString() << "\"" << std::endl; // type: "any", (nextline) (space: thisdeepth)
    }
    ss << "\n" << getSpace(deepth-1) << "}";
    return ss.str();
}

bool xast::rule_parser::isInBuiltInFunctionList(std::string s){
    if( s == "value" ) return true;
    else if( s == "ptr" ) return true;
    else if( s == "and" ) return true;
    else if( s == "or" ) return true;
    else if( s == "not" ) return true;
    else if( s == "lm" ) return true;
    else if( s == "rm" ) return true;
    else return false;
}

xast::rule_parser::memberexpr_parser::memberexpr_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::indexof_parser::indexof_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::arraysubscript_parser::arraysubscript_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::typename_parser::typename_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::primary_parser::primary_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::term_parser::term_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::addexpr_parser::addexpr_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::relexpr_parser::relexpr_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::eqexpr_parser::eqexpr_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::andexpr_parser::andexpr_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::orexpr_parser::orexpr_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::rightexpr_parser::rightexpr_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::argument_parser::argument_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::struct_decl_parser::struct_decl_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::class_decl_parser::class_decl_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::function_call_statement_parser::function_call_statement_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::function_def_stmt_parser::function_def_stmt_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::block_parser::block_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::import_stmt_parser::import_stmt_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::if_stmt_parser::if_stmt_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::for_stmt_parser::for_stmt_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::while_stmt_parser::while_stmt_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::normal_stmt_parser::normal_stmt_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::var_stmt_parser::var_stmt_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::statement_parser::statement_parser(Lexer *lexer){this->lexer = lexer;}

// 右结合表达式
xast::astree xast::rule_parser::rightexpr_parser::match(){
    backup_for_rollback;
    xast::astree left = xast::rule_parser::indexof_parser(lexer).match();
    if(left.matchWithRule == ""){failed_to_match;}
    if(lexer->last.tok_val != tok_eq && lexer->last.tok_val != tok_addwith && lexer->last.tok_val != tok_subwith && lexer->last.tok_val != tok_mulwith && lexer->last.tok_val != tok_divwith && lexer->last.tok_val != tok_modwith && lexer->last.tok_val != tok_colon){
        reset_to_backup; // reset for re-read again
        return xast::rule_parser::orexpr_parser(lexer).match();
    }
    Token op = lexer->last;
    xast::astree right;
    lexer->getNextToken();
    if(op.tok_val == tok_colon){ right = xast::rule_parser::typename_parser(lexer).match(); }
    right = xast::rule_parser::orexpr_parser(lexer).match();
    return xast::astree("rightexpr",{left,astree("operator",op),right});
}

xast::astree xast::rule_parser::orexpr_parser::match(){
    backup_for_rollback;
    xast::astree left = xast::rule_parser::andexpr_parser(lexer).match();
    if(left.matchWithRule == ""){failed_to_match;}
    //if(!(lexer->last.tok_val == tok_add || lexer->last.tok_val == tok_sub)){failed_to_match;}
    while(lexer->last.tok_val == tok_or){
        astree op = astree("operator",lexer->last);
        lexer->getNextToken();
        astree right = xast::rule_parser::andexpr_parser(lexer).match();
        left = astree("expression",{left,op,right});
        //lexer->getNextToken();
    }
    //lexer->getNextToken();
    return left;
}

xast::astree xast::rule_parser::andexpr_parser::match(){
    backup_for_rollback;
    xast::astree left = xast::rule_parser::eqexpr_parser(lexer).match();
    if(left.matchWithRule == ""){failed_to_match;}
    //if(!(lexer->last.tok_val == tok_add || lexer->last.tok_val == tok_sub)){failed_to_match;}
    while(lexer->last.tok_val == tok_and){
        astree op = astree("operator",lexer->last);
        lexer->getNextToken();
        astree right = xast::rule_parser::eqexpr_parser(lexer).match();
        left = astree("expression",{left,op,right});
        //lexer->getNextToken();
    }
    //lexer->getNextToken();
    return left;
}

xast::astree xast::rule_parser::eqexpr_parser::match(){
    backup_for_rollback;
    xast::astree left = xast::rule_parser::relexpr_parser(lexer).match();
    if(left.matchWithRule == ""){failed_to_match;}
    //if(!(lexer->last.tok_val == tok_add || lexer->last.tok_val == tok_sub)){failed_to_match;}
    while(lexer->last.tok_val == tok_equal || lexer->last.tok_val == tok_noteq){
        astree op = astree("operator",lexer->last);
        lexer->getNextToken();
        astree right = xast::rule_parser::relexpr_parser(lexer).match();
        left = astree("expression",{left,op,right});
        //lexer->getNextToken();
    }
    //lexer->getNextToken();
    return left;
}

xast::astree xast::rule_parser::relexpr_parser::match(){
    backup_for_rollback;
    xast::astree left = xast::rule_parser::addexpr_parser(lexer).match();
    if(left.matchWithRule == ""){failed_to_match;}
    //if(!(lexer->last.tok_val == tok_add || lexer->last.tok_val == tok_sub)){failed_to_match;}
    while(lexer->last.tok_val == tok_max || lexer->last.tok_val == tok_min || lexer->last.tok_val == tok_maxeq || lexer->last.tok_val == tok_mineq){
        astree op = astree("operator",lexer->last);
        lexer->getNextToken();
        astree right = xast::rule_parser::addexpr_parser(lexer).match();
        left = astree("expression",{left,op,right});
        //lexer->getNextToken();
    }
    //lexer->getNextToken();
    return left;
}

xast::astree xast::rule_parser::addexpr_parser::match(){
    backup_for_rollback;
    xast::astree left = xast::rule_parser::term_parser(lexer).match();
    if(left.matchWithRule == ""){failed_to_match;}
    //if(!(lexer->last.tok_val == tok_add || lexer->last.tok_val == tok_sub)){failed_to_match;}
    while(lexer->last.tok_val == tok_add || lexer->last.tok_val == tok_sub){
        astree op = astree("operator",lexer->last);
        lexer->getNextToken();
        astree right = xast::rule_parser::term_parser(lexer).match();
        left = astree("expression",{left,op,right});
        //lexer->getNextToken();
    }
    //lexer->getNextToken();
    return left;
}

xast::astree xast::rule_parser::term_parser::match(){
    backup_for_rollback;
    xast::astree left = xast::rule_parser::indexof_parser(lexer).match();
    if(left.matchWithRule == ""){failed_to_match;}
    //if(!(lexer->last.tok_val == tok_mul || lexer->last.tok_val == tok_div || lexer->last.tok_val == tok_mod)){failed_to_match;}
    while(lexer->last.tok_val == tok_mul || lexer->last.tok_val == tok_div || lexer->last.tok_val == tok_mod){
        astree op = astree("operator",lexer->last);
        lexer->getNextToken();
        astree right = xast::rule_parser::indexof_parser(lexer).match();
        left = astree("expression",{left,op,right});
        //lexer->getNextToken();
    }
    //lexer->getNextToken();
    return left;
}

xast::astree xast::rule_parser::memberexpr_parser::match(){
    //backup_for_rollback;
    xast::astree left = xast::rule_parser::primary_parser(lexer).match();
    Token dot = lexer->last; // for get position
    if(lexer->last.tok_val != tok_dot) return left;
    lexer->getNextToken(); // move to next token
    xast::astree right = xast::rule_parser::memberexpr_parser(lexer).match();
    return astree("member_expression",{left,astree("operator",dot),right});
}

xast::astree xast::rule_parser::arraysubscript_parser::match(){
    backup_for_rollback;
    if(lexer->last.str != "["){failed_to_match;}
    lexer->getNextToken();
    xast::astree ast = xast::rule_parser::orexpr_parser(lexer).match();
    if(ast.matchWithRule == ""){failed_to_match;}
    if(lexer->last.str != "]"){failed_to_match;}
    lexer->getNextToken();
    return astree("arraysubscript",{ast});
}

xast::astree xast::rule_parser::indexof_parser::match(){
    backup_for_rollback;
    xast::astree left = xast::rule_parser::memberexpr_parser(lexer).match();
    if(left.matchWithRule == ""){failed_to_match;}
    xast::astree temp;
    temp = xast::rule_parser::arraysubscript_parser(lexer).match();
    if(temp.matchWithRule == "") return left;
    left = astree("indexof",{left}); // 重新构造ast
    while(temp.matchWithRule != ""){
        left.node.push_back(temp);
        temp = xast::rule_parser::arraysubscript_parser(lexer).match();
    }
    return left;
}

xast::astree xast::rule_parser::typename_parser::match(){
    backup_for_rollback;
    if(lexer->last.tok_val != tok_id){failed_to_match;}
    xast::astree ast = xast::astree("typename",lexer->last);
    lexer->getNextToken();
    while(lexer->last.tok_val == tok_mul){
        ast = xast::astree("typename",{xast::astree("ptr_lbl",lexer->last),ast});
        lexer->getNextToken();
    }
    return ast;
}

xast::astree xast::rule_parser::primary_parser::match(){
    backup_for_rollback;
    if(lexer->last.tok_val == tok_int || lexer->last.tok_val == tok_charter || lexer->last.tok_val == tok_string){xast::astree ast=xast::astree("primary",lexer->last);lexer->getNextToken();return ast;}
    else if(lexer->last.tok_val == tok_sbracketl){
        lexer->getNextToken();
        xast::astree expr_match_result = xast::rule_parser::orexpr_parser(lexer).match();
        if(expr_match_result.matchWithRule == ""){
            failed_to_match;
        }
        else if(lexer->last.tok_val != tok_sbracketr){
            failed_to_match;
        }
        else{
            lexer->getNextToken(); // 跳过右括号,一般使用expression不需要这行
            return xast::astree("primary",{expr_match_result});
        }
    }else if(lexer->last.tok_val == tok_mbracketl){
        lexer->getNextToken();
        xast::astree block_result = xast::rule_parser::block_parser(lexer).match();
        lexer->getNextToken();
        return block_result;
    }else if(lexer->last.tok_val == tok_id){
        xast::astree fc_stmt_result = xast::rule_parser::function_call_statement_parser(lexer).match();
        if(fc_stmt_result.matchWithRule != ""){ return xast::astree("primary",{fc_stmt_result}); }
        else if( ( fc_stmt_result = xast::rule_parser::function_def_stmt_parser(lexer).match() ).matchWithRule != "" ){return xast::astree("primary",{fc_stmt_result});}
        else {Token ret = lexer->last;lexer->getNextToken();return xast::astree("primary",ret);} // 没获取到按正常id处理

    }
    failed_to_match;
}

xast::astree xast::rule_parser::argument_parser::match(){
    backup_for_rollback;
    xast::astree ast;
    ast.t = xast::astree::_node;
    ast.matchWithRule = "argument";
    if(lexer->last.tok_val == tok_sbracketr || lexer->last.tok_val == tok_semicolon){return ast;} // 没有参数，返回，小括号右端要匹配，不用跳过
    ast.node.push_back(xast::rule_parser::rightexpr_parser(lexer).match());
    if(ast.node[0].matchWithRule == ""){ast.node.erase(ast.node.begin());return ast;}
    while(lexer->last.tok_val == tok_comma){
        lexer->getNextToken();
        xast::astree newer = xast::rule_parser::rightexpr_parser(lexer).match();
        sendWarning("Empty argument delected.",lexer->last.line,lexer->last.col);
        ast.node.push_back(newer);
    }
    return ast;
}

xast::astree xast::rule_parser::function_def_stmt_parser::match(){
    backup_for_rollback;
    if(lexer->last.tok_val != tok_id || lexer->last.str != "function"){failed_to_match;}
    lexer->getNextToken();
    
    // "function" argument typename "(" argument ")" block
    //            ~~~~~~~~
    xast::astree function_labels = xast::rule_parser::argument_parser(lexer).match();
    //lexer->getNextToken(); argument 会向后读一个token

    // "function" argument typename "(" argument ")" block
    //                     ~~~~~~~~
    xast::astree type = xast::rule_parser::typename_parser(lexer).match();
    if(type.matchWithRule == ""){ throw compiler_error("bad function declaration statement syntax.",lexer->last.line,lexer->last.col); }
    if(lexer->last.tok_val != tok_sbracketl){ throw compiler_error("bad function declaration statement syntax.",lexer->last.line,lexer->last.col); }
    lexer->getNextToken();

    // "function" argument typename "(" argument ")" block
    //                              ~~~~~~~~~~~~~~~~
    xast::astree function_arguments = xast::rule_parser::argument_parser(lexer).match();
    if(lexer->last.tok_val != tok_sbracketr){ throw compiler_error("bad function declaration statement syntax.",lexer->last.line,lexer->last.col); }

    // "function" argument typename "(" argument ")" block
    //                                               ~~~~~
    lexer->getNextToken();// code block
    if(lexer->last.tok_val != tok_mbracketl){throw compiler_error("bad function declaration statement syntax.",lexer->last.line,lexer->last.col);} // 与上注释相同
    lexer->getNextToken();
    xast::astree block = xast::rule_parser::block_parser(lexer).match();
    if(lexer->last.tok_val != tok_mbracketr){throw compiler_error("bad function declaration statement syntax.",lexer->last.line,lexer->last.col);} // 与上注释相同
    lexer->getNextToken();
    if(block.matchWithRule == ""){ throw compiler_error("bad function declaration statement syntax.",lexer->last.line,lexer->last.col); }

    // 组装
    xast::astree ast = xast::astree("function_def_stmt",{function_labels,type,function_arguments,block});
    return ast;

}

xast::astree xast::rule_parser::function_call_statement_parser::match(){
    backup_for_rollback;
    if(lexer->last.tok_val != tok_id){failed_to_match;}
    // 后期在这里加member_expression的解析 Already had

    Token funcname = lexer->last;
    lexer->getNextToken();
    if(lexer->last.tok_val != tok_sbracketl){failed_to_match;}
    lexer->getNextToken();
    xast::astree args = xast::rule_parser::argument_parser(lexer).match();
    if(lexer->last.tok_val != tok_sbracketr){throw compiler_error("expected an ')'",lexer->last.line,lexer->last.col);failed_to_match;}
    lexer->getNextToken(); // 跳过右括号
    return !isInBuiltInFunctionList(funcname.str) ? astree("function_call_statement",{astree("id",funcname),args}) : astree("builtInFunctionCallStmt",{astree("id",funcname),args});    
}

xast::astree xast::rule_parser::block_parser::match(){
    xast::astree root("block",Token());
    xast::astree temp = xast::rule_parser::statement_parser(lexer).match();
    if(temp.matchWithRule == ""){lexer->getNextToken();return root;}
    while(temp.matchWithRule != ""){
        root.node.push_back(temp);
        if(lexer->last.tok_val != tok_semicolon){throw compiler_error("expected a semicolon before next statement.",lexer->last.line,lexer->last.col);} // 与上注释相同
        lexer->getNextToken();
        temp = xast::rule_parser::statement_parser(lexer).match();
    }
    return root;
}

// match function must before function call statement match function
xast::astree xast::rule_parser::import_stmt_parser::match(){
    backup_for_rollback;
    xast::astree ast("import_stmt",Token());
    if(lexer->last.tok_val != tok_id || lexer->last.str != "import"){failed_to_match;}
    lexer->getNextToken();
    if(lexer->last.tok_val == tok_semicolon){throw compiler_error("bad import statement syntax.",lexer->last.line,lexer->last.col);}
    ast.node.push_back(xast::rule_parser::rightexpr_parser(lexer).match());
    if(ast.node[ast.node.size()-1].matchWithRule != "rightexpr"){throw compiler_error("bad import statement syntax.",lexer->last.line,lexer->last.col);}
    while(lexer->last.tok_val == tok_comma){
        if(ast.node[ast.node.size()-1].matchWithRule != "rightexpr"){throw compiler_error("bad import statement syntax.",lexer->last.line,lexer->last.col);}
        lexer->getNextToken();
        xast::astree newer = xast::rule_parser::rightexpr_parser(lexer).match();
        ast.node.push_back(newer);
    }
    return ast;
}

xast::astree xast::rule_parser::if_stmt_parser::match(){
    backup_for_rollback; // must backup
    if(lexer->last.tok_val != tok_id || lexer->last.str != "if"){failed_to_match;}
    lexer->getNextToken();
    if(lexer->last.tok_val != tok_sbracketl){throw compiler_error("bad if statement syntax.",lexer->last.line,lexer->last.col);} // 已经是编译错误了，没必要再保存源坐标,源坐标仅供返回
    lexer->getNextToken();
    xast::astree expr = xast::rule_parser::rightexpr_parser(lexer).match();
    if(expr.matchWithRule == ""){throw compiler_error("bad if statement syntax.",lexer->last.line,lexer->last.col);}
    if(lexer->last.tok_val != tok_sbracketr){throw compiler_error("bad if statement syntax.",lexer->last.line,lexer->last.col);} // 与上注释相同
    lexer->getNextToken();
    // code block
    if(lexer->last.tok_val != tok_mbracketl){throw compiler_error("bad if statement syntax.",lexer->last.line,lexer->last.col);} // 与上注释相同
    lexer->getNextToken();
    xast::astree block = xast::rule_parser::block_parser(lexer).match();
    if(lexer->last.tok_val != tok_mbracketr){throw compiler_error("bad if statement syntax.",lexer->last.line,lexer->last.col);} // 与上注释相同
    lexer->getNextToken();
    xast::astree root("if_stmt",{expr,block});
    while(lexer->last.tok_val == tok_id && lexer->last.str == "else"){
        lexer->getNextToken();
        xast::astree temp = xast::rule_parser::if_stmt_parser(lexer).match(); // 类似于右结合生成
        if(temp.matchWithRule == "") temp = xast::rule_parser::block_parser(lexer).match();
        if(temp.matchWithRule == "") throw compiler_error("bad if statement syntax.",lexer->last.line,lexer->last.col); // 与上注释相同
        root.node.push_back(astree("else",{temp}));
    }
    return root;
}

xast::astree xast::rule_parser::while_stmt_parser::match(){
    backup_for_rollback;
    if(lexer->last.tok_val != tok_id || lexer->last.str != "while"){failed_to_match;}
    lexer->getNextToken();
    if(lexer->last.tok_val != tok_sbracketl) throw compiler_error("bad while statement syntax.",lexer->last.line,lexer->last.col); // 与上注释相同
    lexer->getNextToken();
    xast::astree expr = xast::rule_parser::rightexpr_parser(lexer).match();
    if(lexer->last.tok_val != tok_sbracketr) throw compiler_error("bad while statement syntax.",lexer->last.line,lexer->last.col); // 与上注释相同
    lexer->getNextToken(); 
    if(lexer->last.tok_val != tok_mbracketl) throw compiler_error("bad while statement syntax.",lexer->last.line,lexer->last.col); // 与上注释相同
    lexer->getNextToken(); 
    xast::astree block = xast::rule_parser::block_parser(lexer).match();
    if(lexer->last.tok_val != tok_mbracketr) throw compiler_error("bad while statement syntax.",lexer->last.line,lexer->last.col); // 与上注释相同
    lexer->getNextToken();
    if(block.matchWithRule == "") throw compiler_error("bad while statement syntax.",lexer->last.line,lexer->last.col); // 与上注释相同
    return astree("while_stmt",{expr,block});
}

xast::astree xast::rule_parser::for_stmt_parser::match(){
    backup_for_rollback;
    if(lexer->last.tok_val != tok_id || lexer->last.str != "for"){failed_to_match;}
    lexer->getNextToken();
    if(lexer->last.tok_val != tok_sbracketl) throw compiler_error("bad for statement syntax.",lexer->last.line,lexer->last.col); // 与上注释相同
    lexer->getNextToken();
    xast::astree expr("for_expr_block",Token());
    expr.node.push_back(xast::rule_parser::rightexpr_parser(lexer).match());
    if(expr.node[0].matchWithRule == "") throw compiler_error("bad for statement syntax.",lexer->last.line,lexer->last.col); // 与上注释相同
    if(lexer->last.tok_val != tok_semicolon) throw compiler_error("bad for statement syntax.",lexer->last.line,lexer->last.col); // 与上注释相同
    lexer->getNextToken();

    expr.node.push_back(xast::rule_parser::rightexpr_parser(lexer).match());
    if(expr.node[1].matchWithRule == "") throw compiler_error("bad for statement syntax.",lexer->last.line,lexer->last.col); // 与上注释相同
    if(lexer->last.tok_val != tok_semicolon) throw compiler_error("bad for statement syntax.",lexer->last.line,lexer->last.col); // 与上注释相同
    lexer->getNextToken();
    
    expr.node.push_back(xast::rule_parser::rightexpr_parser(lexer).match());
    if(expr.node[2].matchWithRule == "") throw compiler_error("bad for statement syntax.",lexer->last.line,lexer->last.col); // 与上注释相同
    
    if(lexer->last.tok_val != tok_sbracketr) throw compiler_error("bad for statement syntax.",lexer->last.line,lexer->last.col); // 与上注释相同
    lexer->getNextToken();
    if(lexer->last.tok_val != tok_mbracketl) throw compiler_error("bad for statement syntax.",lexer->last.line,lexer->last.col); // 与上注释相同
    lexer->getNextToken();
    xast::astree block = xast::rule_parser::block_parser(lexer).match();
    //lexer->getNextToken();
    if(lexer->last.tok_val != tok_mbracketr) throw compiler_error("bad for statement syntax.",lexer->last.line,lexer->last.col); // 与上注释相同
    lexer->getNextToken();
    return astree("for_stmt",{expr,block});
}

// TODO:结构体声明
xast::astree xast::rule_parser::struct_decl_parser::match(){
    backup_for_rollback;
    if(lexer->last.tok_val != tok_id || lexer->last.str != "struct"){failed_to_match;}
    lexer->getNextToken();
    xast::astree ast("struct_decl",{xast::rule_parser::primary_parser(lexer).match()});
    //lexer->getNextToken();
    if(lexer->last.tok_val != tok_mbracketl){ throw compiler_error("bad struct declaration.",lexer->last.line,lexer->last.col); }
    lexer->getNextToken();
    ast.node.push_back(xast::rule_parser::block_parser(lexer).match());
    if(ast.node[ast.node.size() - 1].matchWithRule == ""){ throw compiler_error("bad struct declaration.",lexer->last.line,lexer->last.col); }
    if(lexer->last.tok_val != tok_mbracketr){ throw compiler_error("bad struct declaration.",lexer->last.line,lexer->last.col); }
    lexer->getNextToken(); // now is ';'
    return ast;
}

// TODO:类声明
xast::astree xast::rule_parser::class_decl_parser::match(){
    backup_for_rollback;
    if(lexer->last.tok_val != tok_id || lexer->last.str != "class"){failed_to_match;}
    lexer->getNextToken();
    xast::astree ast("class_decl",{xast::rule_parser::rightexpr_parser(lexer).match()}); // class _childClassName:fatherClassName { stmts };
    //lexer->getNextToken();
    if(lexer->last.tok_val != tok_mbracketl){ throw compiler_error("bad class declaration.",lexer->last.line,lexer->last.col); }
    lexer->getNextToken();
    ast.node.push_back(xast::rule_parser::block_parser(lexer).match());
    if(ast.node[ast.node.size() - 1].matchWithRule == ""){ throw compiler_error("bad class declaration.",lexer->last.line,lexer->last.col); }
    if(lexer->last.tok_val != tok_mbracketr){ throw compiler_error("bad class declaration.",lexer->last.line,lexer->last.col); }
    lexer->getNextToken(); // now is ';'
    return ast;
}

// TODO:一切带关键词语句的匹配
xast::astree xast::rule_parser::normal_stmt_parser::match(){
    backup_for_rollback;
    xast::astree ast("normal_stmt_" + lexer->last.str,Token());
    if(lexer->last.tok_val != tok_id || (lexer->last.str != "return" && lexer->last.str != "var" && lexer->last.str != "continue" && lexer->last.str != "break")){failed_to_match;}
    if(lexer->last.str == "return"){
        lexer->getNextToken();
        ast.node.push_back(xast::rule_parser::rightexpr_parser(lexer).match());
        if(ast.node[0].matchWithRule == "") throw compiler_error("bad return statement syntax",lexer->last.line,lexer->last.col);
    }
    // don't need getnexttoken because rightexpr already processed , here is a semicolon
    return ast;
}

// TODO: var declaration process
// 指针与cpp不同，注意使用
xast::astree xast::rule_parser::var_stmt_parser::match(){
    backup_for_rollback;
    xast::astree var_method,type,args;
    if(lexer->last.tok_val == tok_id && lexer->last.str == "public"){ var_method = xast::astree("var_method",lexer->last);lexer->getNextToken(); }
    else if(lexer->last.tok_val == tok_id && lexer->last.str == "private"){ var_method = xast::astree("var_method",lexer->last);lexer->getNextToken(); }
    else { var_method = xast::astree("var_method",Token(tok_id,"public",lexer->last.line,lexer->last.col)); } // default as public
    type = xast::rule_parser::typename_parser(lexer).match();
    if(type.matchWithRule == ""){failed_to_match;}
    args = xast::rule_parser::argument_parser(lexer).match();
    if(args.matchWithRule == "" || args.node.empty()){ failed_to_match; }
    return xast::astree("var_stmt",{var_method,type,args});
}

// 因为blockstatement的解析，所以statement并不需要semicolon的解析
xast::astree xast::rule_parser::statement_parser::match(){
    xast::astree current;
    if(lexer->last.tok_val == tok_semicolon) return current; // empty statement
    current = xast::rule_parser::struct_decl_parser(lexer).match();
    if(current.matchWithRule != "") return current;
    current = xast::rule_parser::class_decl_parser(lexer).match();
    if(current.matchWithRule != "") return current;
    current = xast::rule_parser::import_stmt_parser(lexer).match();
    if(current.matchWithRule != "") return current;
    current = xast::rule_parser::if_stmt_parser(lexer).match();
    if(current.matchWithRule != "") return current;
    current = xast::rule_parser::while_stmt_parser(lexer).match();
    if(current.matchWithRule != "") return current;
    current = xast::rule_parser::for_stmt_parser(lexer).match();
    if(current.matchWithRule != "") return current;
    current = xast::rule_parser::function_call_statement_parser(lexer).match();
    if(current.matchWithRule != "") return current;
    current = xast::rule_parser::normal_stmt_parser(lexer).match();
    if(current.matchWithRule != "") return current;
    current = xast::rule_parser::var_stmt_parser(lexer).match();
    if(current.matchWithRule != "") return current; // god said, the priority level of var stmt must higher than expression
    current = xast::rule_parser::rightexpr_parser(lexer).match();
    if(current.matchWithRule == "expression") sendWarning("An expression that has no effect on the program is calculated.",lexer->last.line,lexer->last.col);
    if(current.matchWithRule != "") return current;
    return current; // an empty value
}