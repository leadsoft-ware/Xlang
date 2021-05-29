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
        astree(){}
        astree(std::string matchWithRule,Token t);
        astree(std::string matchWithRule,std::vector<astree> n);
    };

    // user customize , 如果没有匹配的token，请将pos,line,col三个变量拨回原始的地方
    namespace rule_parser{
        class primary_parser{
            Lexer *lexer;
            public:
            primary_parser(Lexer *lexer);
            astree match(); // 不匹配返回空树
        };
        class expression_parser{
            Lexer *lexer;
            public:
            expression_parser(Lexer *lexer);
            astree match(); // 不匹配返回空树
        };
        class block_parser{
            Lexer *lexer;
            public:
            block_parser(Lexer *lexer);
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
