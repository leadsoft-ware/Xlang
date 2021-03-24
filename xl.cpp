#include "compiler/compiler.cpp"

std::map<std::string,std::string> arg;

void parseArgs(int argc,char** argv){
    for(int i = 1;i<argc;i++){
        Lexer lex(argv[i]);
        ASTree ast(lex);
        if(ast.nodeT == ExpressionStatement){
            arg[ast.node[0].this_node.str] = ast.node[1].this_node.str;
        }else{
            arg[ast.this_node.str] = "true";
        }
    }
}

void printArgs(){
    for(auto i = arg.begin(); i != arg.end();i ++){
        std::cout << i->first << "->" << i->second << std::endl;
    }
}

int main(int argc,char** argv){
    parseArgs(argc,argv);
    printArgs();
    freopen("compile_log.log","w",stdout);
    std::string code;
    std::ofstream output("compile_log.log");
    std::string filename = arg["input_filename"];
    if(filename == "") return 1;
    std::ifstream infile; 
    infile.open(filename);
    std::cout << "Xlang Compiler Alpha 0.1\n";
    std::cout << "Powered by xiaokang00010\n";
    std::cout << "Filename -> " << filename <<  std::endl;
    std::cout << "Reading Program to buffer...";
    while(!infile.eof()){
        std::string tmpline;
        getline(infile,tmpline);
        code += tmpline + "\n";
    }
    std::cout << code <<  std::endl;
    InitCompiler();
    std::vector<ASMBlock> asms;
    asms = CompileProcess(code);
    for(int i = 0;i < asms.size();i=i+1){
        std::cout << asms[i].tostring();
    }
    Bytecode::Init(asms);
    Bytecode::exportBytecode();
    VMExec_Serialization((char*)std::string(arg["output_filename"]).c_str(),Bytecode::packVMExec());
    std::cout <<  std::endl;
}