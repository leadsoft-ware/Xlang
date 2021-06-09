#include "../include/xasm.cpp"
#include <map>
#include <iomanip>
using namespace std;
map<string,string> flags;


void parse_args(int argc,const char** argv){
    if(argc == 1) return;
    for(int i = 1;i < argc;i++){
        Lexer lexer(argv[i]);
        lexer.getNextToken();
        xast::astree ast = xast::rule_parser::rightexpr_parser(&lexer).match();
        if(ast.matchWithRule == ""){throw compiler_error(string("unknown argument at ")+ argv[i],lexer.line,lexer.col);}
        else if(ast.matchWithRule == "rightexpr") flags[ast.node[0].tok.str] = ast.node[2].tok.str;
        else if(ast.matchWithRule == "primary") flags[ast.tok.str] = "true";
        else{throw compiler_error(string("unknown argument at ")+ argv[i],lexer.line,lexer.col);}
    }
}

void terminal(){
    while(true){
        try{
            std::string str;
            cout << "(xlang-insider@terminal) -> ";
            getline(cin,str);
            cout << "input:" << str << endl;
            Lexer lexer(str);
            lexer.getNextToken();
            xast::astree ast = xast::rule_parser::asm_block_stmt_parser(&lexer).match();
            xasm::asm_block block = xasm::translateToASMStruct(ast);
            xasm::bytecode_block bytecodes = xasm::translateToBytecode(block);
            cout << ast.toString(1) << endl;
        }catch(compiler_error e){
            cout << e.what();
        }
    }
}

void with_file(){

}

int main(int argc,const char** argv){
    cout << "[Xasm] Version " << __xlang_version << " build " << __xlang_build << "\n";
    #ifdef __xlang_insider
    cout << "Insider Preview: This product is not for personal users. Many features are not complete in this version." << endl;
    #endif
    try{
        parse_args(argc,argv);
        /*for(auto i = flags.begin();i != flags.end();i++){
            cout << "arg:" << i->first << ":" << i->second << setw(4);
        }*/
        if(flags["terminal"] == "true") terminal();
        else if(flags["file"] == "true") with_file();
        else throw compiler_error("unknown compiler mode",0,0);
    }catch(compiler_error e){
        cout << e.what();
    };
    string s;
    
}