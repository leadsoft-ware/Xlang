#include "compiler/compiler.cpp"
using namespace std;

int main(){
    string code;
    ofstream output("compile_log.log");
    string filename;
    getline(cin,filename);
    if(filename == "") return 1;
    ifstream infile; 
    infile.open(filename);
    output << "Xlang Compiler Alpha 0.1\n";
    output << "Powered by xiaokang00010\n";
    output << "Filename -> " << filename << endl;
    output << "Reading Program to buffer...";
    while(!infile.eof()){
        string tmpline;
        getline(infile,tmpline);
        code += tmpline + "\n";
    }
    output << code << endl;
    InitCompiler();
    vector<ASMBlock> asms;
    asms = CompileProcess(code);
    for(int i = 0;i < asms.size();i=i+1){
        cout << asms[i].toString();
    }
    Bytecode::Init(asms);
    Bytecode::exportBytecode();
    VMExec_Serialization("test.xvm",Bytecode::packVMExec());
    cout << endl;
}