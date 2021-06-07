#include "../../include/xast.cpp"
#include "asm_headers.cpp"

namespace xast::rule_parser{
    class asm_stmt_parser{
        Lexer *lexer;
        public:
        asm_stmt_parser(Lexer *lexer){this->lexer = lexer;}
        // 匹配一行ast语句
        xast::astree match(){
            backup_for_rollback;
            if(lexer->last.tok_val != tok_id || !xasm::iskeyword(lexer->last.str)){failed_to_match;}
            xast::astree root("asm_"+lexer->last.str,Token());

            lexer->getNextToken();
            if(lexer->last.tok_val != tok_sbracketl){throw compiler_error("expected an '(' befoce arguments",lexer->line,lexer->col);}
            lexer->getNextToken();
            xast::astree args = xast::rule_parser::argument_parser(lexer).match();
            if(lexer->last.tok_val != tok_sbracketr){throw compiler_error("expected an ')' after arguments",lexer->line,lexer->col);}
            lexer->getNextToken();

            if(args.matchWithRule == ""){root.t = xast::astree::child;return root;}
            root.node.push_back(args);
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
            if(lexer->last.tok_val != tok_mbracketl){throw compiler_error("expected a '{' befoce block.",lexer->line,lexer->col);} // 与上注释相同
            lexer->getNextToken();

            xast::astree temp = xast::rule_parser::asm_stmt_parser(lexer).match();
            if(temp.matchWithRule == ""){lexer->getNextToken();return root;}
            while(temp.matchWithRule != ""){
                root.node.push_back(temp);
                if(lexer->last.tok_val != tok_semicolon){throw compiler_error("expected a semicolon before next statement.",lexer->line,lexer->col);} // 与上注释相同
                lexer->getNextToken();
                temp = xast::rule_parser::asm_stmt_parser(lexer).match();
            }
            if(lexer->last.tok_val != tok_mbracketr){throw compiler_error("expected a '}' after block.",lexer->line,lexer->col);} // 与上注释相同
            lexer->getNextToken();

            return root;
        }
    };
};

namespace xasm{
    // 翻译
    asm_block translateToASMStruct(xast::astree stmt){
        asm_block ret;
        if(stmt.matchWithRule == "asmblock"){
            ret.block_name = stmt.node[0].tok.str;
            for(int i = 1;i < stmt.node.size();i++) ret.merge(translateToASMStruct(stmt.node[i]));
            return ret;
        }else if(stmt.matchWithRule.substr(0,4) == "asm_"){
            std::vector<std::string> args;
            for(int i = 0;i < stmt.node[0].node.size();i++){
                if(stmt.node[0].node[i].matchWithRule != "primary"){throw compiler_error("[unknown position] expected an primary token.",0,0);} // 与上注释相同
                args.push_back(stmt.node[0].node[i].tok.str);
            }
            asm_command c = (asm_command){stmt.matchWithRule.substr(4),args};
            ret.code.push_back(c);
            return ret;
        }else{
            throw compiler_error("[unknown position] unknown asm match rule.",0,0);
        }
    }
};