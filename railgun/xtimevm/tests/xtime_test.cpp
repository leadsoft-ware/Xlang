//#include "../../xasm/include/xasm.cpp"
#include "../include/xtime.cpp"

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

int main(int argc,const char** argv){
    cout << "[XtimeVM] Version " << __xlang_version << " build " << __xlang_build << "\n";
    #ifdef __xlang_insider
    cout << "Insider Preview: This product is not for personal users. Many features are not complete in this version." << endl;
    #endif
    /*try{*/
        parse_args(argc,argv);
        for(auto i = flags.begin();i != flags.end();i++){
            cout << "arg:" << i->first << ":" << i->second << " ";
        }
        cout << "arg end" << endl;
        virtual_machine vm(xasm::open_xmvef_file((char*)flags["file"].c_str()),stol(flags["memsize"]));
        vm.start();
    /*}catch(compiler_error e){
        cout << e.what();
    };*/
    string s;
    
}