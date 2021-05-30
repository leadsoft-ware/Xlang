/*
    Description:
    xast实现
*/

#include "xast.hpp"

#define failed_to_match lexer->line = line,lexer->col = col,lexer->pos = pos; return astree()

xast::astree::astree(){
    matchWithRule = "";
    this->t = child;
}

// init for child
xast::astree::astree(std::string matchWithRule,Token t){
    this->t = child;
    this->matchWithRule = matchWithRule;
    this->tok = t;
}

xast::astree::astree(std::string matchWithRule,std::vector<astree> n){
    this->t = leaf;
    this->matchWithRule = matchWithRule;
    this->node = n;
}

xast::rule_parser::primary_parser::primary_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::term_parser::term_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::addexpr_parser::addexpr_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::relexpr_parser::relexpr_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::eqexpr_parser::eqexpr_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::andexpr_parser::andexpr_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::orexpr_parser::orexpr_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::argument_parser::argument_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::function_call_statement_parser::function_call_statement_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::if_stmt_parser::if_stmt_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::for_stmt_parser::for_stmt_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::while_stmt_parser::while_stmt_parser(Lexer *lexer){this->lexer = lexer;}

xast::rule_parser::statement_parser::statement_parser(Lexer *lexer){this->lexer = lexer;}

xast::astree xast::rule_parser::orexpr_parser::match(){
    int line = lexer->line,col = lexer->col,pos = lexer->pos; // backup for rollback
    xast::astree left = xast::rule_parser::andexpr_parser(lexer).match();
    //if(!(lexer->last.tok_val == tok_add || lexer->last.tok_val == tok_sub)){failed_to_match;}
    while(lexer->last.tok_val == tok_and){
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
    int line = lexer->line,col = lexer->col,pos = lexer->pos; // backup for rollback
    xast::astree left = xast::rule_parser::eqexpr_parser(lexer).match();
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
    int line = lexer->line,col = lexer->col,pos = lexer->pos; // backup for rollback
    xast::astree left = xast::rule_parser::relexpr_parser(lexer).match();
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
    int line = lexer->line,col = lexer->col,pos = lexer->pos; // backup for rollback
    xast::astree left = xast::rule_parser::addexpr_parser(lexer).match();
    //if(!(lexer->last.tok_val == tok_add || lexer->last.tok_val == tok_sub)){failed_to_match;}
    while(lexer->last.tok_val == tok_max || lexer->last.tok_val == tok_min){
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
    int line = lexer->line,col = lexer->col,pos = lexer->pos; // backup for rollback
    xast::astree left = xast::rule_parser::term_parser(lexer).match();
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
    int line = lexer->line,col = lexer->col,pos = lexer->pos; // backup for rollback
    xast::astree left = xast::rule_parser::primary_parser(lexer).match();
    //if(!(lexer->last.tok_val == tok_mul || lexer->last.tok_val == tok_div || lexer->last.tok_val == tok_mod)){failed_to_match;}
    while(lexer->last.tok_val == tok_mul || lexer->last.tok_val == tok_div || lexer->last.tok_val == tok_mod){
        astree op = astree("operator",lexer->last);
        lexer->getNextToken();
        astree right = xast::rule_parser::primary_parser(lexer).match();
        left = astree("expression",{left,op,right});
        //lexer->getNextToken();
    }
    //lexer->getNextToken();
    return left;
}

xast::astree xast::rule_parser::primary_parser::match(){
    int line = lexer->line,col = lexer->col,pos = lexer->pos; // backup for rollback
    if(lexer->last.tok_val == tok_int || lexer->last.tok_val == tok_charter || lexer->last.tok_val == tok_string){xast::astree ast=xast::astree("primary",lexer->last);lexer->getNextToken();return ast;}
    else if(lexer->last.tok_val == tok_sbracketl){
        lexer->getNextToken();
        xast::astree expr_match_result = xast::rule_parser::addexpr_parser(lexer).match();
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
    }else if(lexer->last.tok_val == tok_id){
        xast::astree fc_stmt_result = xast::rule_parser::function_call_statement_parser(lexer).match();
        if(fc_stmt_result.matchWithRule == ""){lexer->getNextToken();return xast::astree("primary",lexer->last);} // 没获取到按正常id处理
        else return xast::astree("primary",{fc_stmt_result});
    }
    failed_to_match;
}

xast::astree xast::rule_parser::argument_parser::match(){
    int line = lexer->line,col = lexer->col,pos = lexer->pos; // backup for rollback
    xast::astree ast;
    ast.matchWithRule = "argument";
    if(lexer->last.tok_val == tok_sbracketr){return ast;} // 没有参数，返回，小括号右端要匹配，不用跳过
    ast.node.push_back(xast::rule_parser::addexpr_parser(lexer).match());
    while(lexer->last.tok_val == tok_colon){
        lexer->getNextToken();
        xast::astree newer = xast::rule_parser::addexpr_parser(lexer).match();
        ast.node.push_back(newer);
    }
    return ast;
}

xast::astree xast::rule_parser::function_call_statement_parser::match(){
    int line = lexer->line,col = lexer->col,pos = lexer->pos; // backup for rollback
    if(lexer->last.tok_val != tok_id){failed_to_match;}
    // TODO: 后期在这里加member_expression的解析

    Token funcname = lexer->last;
    lexer->getNextToken();
    if(lexer->last.tok_val != tok_sbracketl){failed_to_match;}
    lexer->getNextToken();
    xast::astree args = xast::rule_parser::argument_parser(lexer).match();
    if(lexer->last.tok_val != tok_sbracketr){throw compiler_error("execpted an ')'",lexer->line+1,lexer->col+1);failed_to_match;}
    lexer->getNextToken(); // 跳过右括号
    return astree("function_call_statement",{astree("id",funcname),args});
}