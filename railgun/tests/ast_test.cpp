#include "../include/xast.cpp"
#include <map>
#include <iomanip>
#include <time.h>
using namespace std;
map< string,vector<string> > flags;

void parse_args(int argc,const char** argv){
    if(argc == 1) return;
    for(int i = 1;i < argc;i++){
        Lexer lexer(argv[i]);
        lexer.getNextToken();
        xast::astree ast = xast::rule_parser::rightexpr_parser(&lexer).match();
        if(ast.matchWithRule == ""){throw compiler_error(string("unknown argument at ")+ argv[i],lexer.line,lexer.col);}
        else if(ast.matchWithRule == "rightexpr"){
            if(ast.node[1].tok.tok_val == tok_eq){flags[ast.node[0].tok.str].push_back(ast.node[2].tok.str);}
            if(ast.node[1].tok.tok_val == tok_addwith) flags[ast.node[0].tok.str].push_back(ast.node[2].tok.str);
        }
        else if(ast.matchWithRule == "primary") {flags[ast.tok.str].push_back("true");}
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
            xast::astree ast = xast::rule_parser::block_parser(&lexer).match();
            cout << ast.toString(1) << endl;
        }catch(compiler_error e){
            cout << e.what();
        }
    }
}

void with_file(){

}

std::string getBuildTime(){
    stringstream ss;
    time_t tt;
    ss << __xlang_build;
    ss >> tt;
    char result[50];
    struct tm * timeSet = gmtime(&tt);

    strftime(result,50, "%Y-%m-%d", timeSet);
    return result;
}

int main(int argc,const char** argv){
    cout << "[Xlang] Version " << __xlang_version << " build_time " << getBuildTime() << "\n";
    #ifdef __xlang_insider
    cout << "Insider Preview: This product is not for personal users. Many features are not complete in this version." << endl;
    #endif
    try{
        parse_args(argc,argv);
        for(auto i = flags.begin();i != flags.end();i++){
            cout << "arg:" << i->first << ": {";
            for(int j = 0;j < i->second.size();j++){
                if(j == i->second.size() - 1) cout << i->second[j] << "}";
                else cout << i->second[j] << ",";
            }
        }
        cout << "arg end" << endl;
        if(flags["terminal"].at(0) == "true") terminal();
        else if(flags["file"].at(0) == "true") with_file();
        else throw compiler_error("unknown compiler mode",0,0);
    }catch(compiler_error e){
        cout << e.what();
    };
    string s;
    
}