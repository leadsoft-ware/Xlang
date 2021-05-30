/*
 Desscription:
 this is the parser and ast library of xlang, product by xiaokang00010
*/
#include "lexer.cpp"

#include <vector>
#include <array>

namespace xast{
    class astree{
        public:
        enum tree_type{leaf,child} t;
        std::string matchWithRule;
        Token tok;
        std::vector<astree> node;
        astree();
        astree(std::string matchWithRule,Token t);
        astree(std::string matchWithRule,std::vector<astree> n);
    };

    // user customize , 如果没有匹配的token，请将pos,line,col三个变量拨回原始的地方
    namespace rule_parser{
        class memberexpr_parser{
            Lexer *lexer;
            public:
            memberexpr_parser(Lexer *lexer);
            astree match(); // 不匹配返回空树
        };
        class primary_parser{
            Lexer *lexer;
            public:
            primary_parser(Lexer *lexer);
            astree match(); // 不匹配返回空树
        };
        class term_parser{
            Lexer *lexer;
            public:
            term_parser(Lexer *lexer);
            astree match(); // 不匹配返回空树
        };
        class addexpr_parser{
            Lexer *lexer;
            public:
            addexpr_parser(Lexer *lexer);
            astree match(); // 不匹配返回空树
        };
        class relexpr_parser{
            Lexer *lexer;
            public:
            relexpr_parser(Lexer *lexer);
            astree match(); // 不匹配返回空树
        };
        class eqexpr_parser{
            Lexer *lexer;
            public:
            eqexpr_parser(Lexer *lexer);
            astree match(); // 不匹配返回空树
        };
        class andexpr_parser{
            Lexer *lexer;
            public:
            andexpr_parser(Lexer *lexer);
            astree match(); // 不匹配返回空树
        };
        class orexpr_parser{
            Lexer *lexer;
            public:
            orexpr_parser(Lexer *lexer);
            astree match(); // 不匹配返回空树
        };
        class argument_parser{
            Lexer *lexer;
            public:
            argument_parser(Lexer *lexer);
            astree match(); // 不匹配返回空树
        };
        class block_parser{
            Lexer *lexer;
            public:
            block_parser(Lexer *lexer);
            astree match(); // 不匹配返回空树
        };
        class function_call_statement_parser{
            Lexer *lexer;
            public:
            function_call_statement_parser(Lexer *lexer);
            astree match(); // 不匹配返回空树
        };
        class if_stmt_parser{
            Lexer *lexer;
            public:
            if_stmt_parser(Lexer *lexer);
            astree match(); // 不匹配返回空树
        };
        class while_stmt_parser{
            Lexer *lexer;
            public:
            while_stmt_parser(Lexer *lexer);
            astree match(); // 不匹配返回空树
        };
        class for_stmt_parser{
            Lexer *lexer;
            public:
            for_stmt_parser(Lexer *lexer);
            astree match(); // 不匹配返回空树
        };
        // 变量定义，返回，等语句
        class normal_stmt_parser{
            Lexer *lexer;
            public:
            normal_stmt_parser(Lexer *lexer);
            astree match(); // 不匹配返回空树
        };
        class statement_parser{
            Lexer *lexer;
            public:
            statement_parser(Lexer *lexer);
            astree match(); // 不匹配返回空树
        };
    };
    astree parse_ast(Lexer &lexer);
};
