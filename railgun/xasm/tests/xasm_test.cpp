#include "../include/xasm.cpp"
#include <sys/stat.h>
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
    if(flags["log_level"] == "full") sendLog(flags["log_level"],"trying open file");
    int fd = open(flags["file"].c_str(),O_RDWR);
    if(fd == -1) throw compiler_error("open file failed",0,0);
    struct stat file_info;
    if(flags["log_level"] == "full") sendLog(flags["log_level"],"trying stat file");
    stat(flags["file"].c_str(),&file_info);
    if(flags["log_level"] == "full") sendLog(flags["log_level"],"allocating memory for code");
    char* s = (char*)malloc(file_info.st_size+1);
    read(fd,s,file_info.st_size);
    Lexer lexer(s);
    //xast::astree ast("root",Token());
    std::vector<xasm::bytecode_block> blocks;
    // ast generate
    if(flags["log_level"] == "full") sendLog(flags["log_level"],"start parse ast");
    while(!(lexer.getNextToken().tok_val == tok_eof)){
        xast::astree ast = xast::rule_parser::asm_block_stmt_parser(&lexer).match();
        if(ast.matchWithRule == "") throw compiler_error("failed to parse ast",lexer.line,lexer.col);
        if(flags["log_level"] == "full") sendLog(flags["log_level"],"ast generated");
        xasm::asm_block temp_block = xasm::translateToASMStruct(ast);
        if(flags["log_level"] == "full") sendLog(flags["log_level"],"transated to asm_struct");
        xasm::bytecode_block b_block = xasm::translateToBytecode(temp_block);
        if(flags["log_level"] == "full") sendLog(flags["log_level"],"generated bytecode block");
        blocks.push_back(b_block);
    }
    if(flags["log_level"] == "full") sendLog(flags["log_level"],"bytecode_block all generated");
}

int main(int argc,const char** argv){
    cout << "[Xasm] Version " << __xlang_version << " build " << __xlang_build << "\n";
    #ifdef __xlang_insider
    cout << "Insider Preview: This product is not for personal users. Many features are not complete in this version." << endl;
    #endif
    try{
        parse_args(argc,argv);
        for(auto i = flags.begin();i != flags.end();i++){
            cout << "arg:" << i->first << ":" << i->second << " ";
        }
        if(flags["terminal"] == "true") terminal();
        else if(flags["file"] != "") with_file();
        else throw compiler_error("unknown compiler mode",0,0);
    }catch(compiler_error e){
        cout << e.what();
    };
    string s;
    
}