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
            xast::astree root(lexer->last.str,Token());
            lexer->getNextToken();
            xast::astree args = xast::rule_parser::argument_parser(lexer).match();
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
            xast::astree root("block",Token());
            xast::astree temp = xast::rule_parser::asm_stmt_parser(lexer).match();
            if(temp.matchWithRule == ""){lexer->getNextToken();return root;}
            while(temp.matchWithRule != ""){
                root.node.push_back(temp);
                if(lexer->last.tok_val != tok_semicolon){throw compiler_error("expected a semicolon before next statement.",lexer->line,lexer->col);} // 与上注释相同
                lexer->getNextToken();
                temp = xast::rule_parser::asm_stmt_parser(lexer).match();
            }
            return root;
        }
    };
};

