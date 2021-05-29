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

xast::rule_parser::primary_parser::primary_parser(Lexer *lexer){
    this->lexer = lexer;
}

xast::rule_parser::expression_parser::expression_parser(Lexer *lexer){
    this->lexer = lexer;
}

xast::rule_parser::term_parser::term_parser(Lexer *lexer){
    this->lexer = lexer;
}

xast::astree xast::rule_parser::expression_parser::match(){
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
    return left;
}

xast::astree xast::rule_parser::primary_parser::match(){
    int line = lexer->line,col = lexer->col,pos = lexer->pos; // backup for rollback
    if(lexer->last.tok_val == tok_int || lexer->last.tok_val == tok_charter || lexer->last.tok_val == tok_string || lexer->last.tok_val == tok_id){xast::astree ast=xast::astree("primary",lexer->last);lexer->getNextToken();return ast;}
    else if(lexer->last.tok_val == tok_sbracketl){
        xast::astree expr_match_result = xast::rule_parser::expression_parser(lexer).match();
        if(expr_match_result.matchWithRule == ""){
            failed_to_match;
        }
        else if(lexer->getNextToken().tok_val != tok_sbracketr){
            failed_to_match;
        }
        else{
            lexer->getNextToken();
            return astree("primary",{expr_match_result});
        }
    }
    failed_to_match;
}