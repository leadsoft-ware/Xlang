#include "compiler/compiler.cpp"
 

int main(){
    freopen("compile_log.log","w",stdout);
    std::string code;
    std::ofstream output("compile_log.log");
    std::string filename;
    getline( std::cin,filename);
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
    VMExec_Serialization("test.xvm",Bytecode::packVMExec());
    std::cout <<  std::endl;
}