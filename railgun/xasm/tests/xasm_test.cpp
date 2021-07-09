#include "../include/xasm.cpp"
#include <sys/stat.h>
#include <map>
#include <iomanip>
#include <time.h>
using namespace std;
map<string,string> flags;
std::vector<xasm::bytecode_block> blocks;


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
    std::vector<xasm::bytecode_block> block_map;
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
            xasm::bytecode_block bytecodes = xasm::translateToBytecode(block,block_map);
            cout << ast.toString(1) << endl;
            block_map.push_back(bytecodes);
        }catch(compiler_error e){
            cout << e.what();
        }
    }
}

void save_xmvef(){
    if(flags["log_level"] == "full" || flags["log_level"] == "normal") sendLog(flags["log_level"],"bytecode_block all generated");
    xasm::xmvef_file xmvef;
    xmvef.head.license=xasm::vmexec_file_header::_gpl;
    xmvef.head.xmvef_sign = 0x114514ff;
    
    std::vector<xasm::bytecode> code;
    for(int i = 0;i < blocks.size();i++){
        for(int j = 0;j < blocks[i].code.size();j++){
            code.push_back(blocks[i].code[j]);
        }
    }
    xmvef.head.bytecode_length = code.size();

    xmvef.head.default_constant_pool_size = xasm::database.size();
    xmvef.constant_pool = (char*)xasm::database.data();
    xmvef.bytecode_array = (xasm::bytecode*)code.data();
    xmvef.head.start_of_pc = xasm::database.size() + xasm::countBytecodeBlock(blocks,0,xasm::inBlockMap("entry",blocks)) * sizeof(xasm::bytecode);
    xmvef.head.from_xlang_package_server = 0;

    if(flags["output"] != "") xasm::create_xmvef_file((char*)flags["output"].data(),xmvef);
    else throw compiler_error("Cannot output to file",0,0);
}

void with_file(std::string filename){
    if(flags["log_level"] == "full") sendLog(flags["log_level"],"trying open file");
    int fd = open(filename.c_str(),O_RDWR);
    if(fd == -1) throw compiler_error("open file failed",0,0);
    struct stat file_info;
    if(flags["log_level"] == "full") sendLog(flags["log_level"],"trying stat file");
    stat(flags["file"].c_str(),&file_info);
    if(flags["log_level"] == "full") sendLog(flags["log_level"],"allocating memory for code");
    std::string s;
    s.resize(file_info.st_size+2);
    read(fd,(char*)s.data(),file_info.st_size);
    // 防止傻逼awathefox抬杠
    s[file_info.st_size+1] = '\0';
    s[file_info.st_size] = '\0';
    Lexer lexer(s);
    //xast::astree ast("root",Token());
    // ast generate
    if(flags["log_level"] == "full" || flags["log_level"] == "normal") sendLog(flags["log_level"],"start parse ast");
    while(lexer.getNextToken().tok_val != tok_eof){
        xast::astree ast = xast::rule_parser::asm_main_stmt_parser(&lexer).match();
        if(ast.matchWithRule == "") throw compiler_error("failed to parse ast",lexer.line,lexer.col);
        if(ast.matchWithRule == "import_stmt"){
            if(flags["log_level"] == "full") sendLog(flags["log_level"],"got a import statement.");
            for(int i = 0;i < ast.node.size();i++){
                if(ast.node[i].node[2].tok.str == "xasm"){
                    with_file(ast.node[i].node[0].tok.str);
                }else if(ast.node[i].node[2].tok.str == "xlang"){
                    sendWarning("Xasm is not support include xlang yet.",0,0);
                }
            }
            continue;
        }
        if(ast.matchWithRule == "asm_marco_stmt"){
            if(flags["log_level"] == "full") sendLog(flags["log_level"],"got a marco statement.");
            xasm::marcos[ast.node[0].tok.str] = ast.node[1];
            continue;
        }
        if(ast.matchWithRule == "asm_db_size_stmt"){
            xasm::database.resize(stol(ast.node[0].tok.str));
            continue;
        }
        if(ast.matchWithRule == "asm_db_stmt"){
            long long addr = stol(ast.node[0].tok.str);
            std::string &toWrite = ast.node[1].tok.str;
            xasm::content temp(0);
            if(is_number(toWrite)){
                temp = stol(toWrite);
                for(int i = 0;i < 8;i++) xasm::database[addr + i] = temp._charval[i];
            }else if(is_decimal(toWrite)){
                temp = stod(toWrite);
                for(int i = 0;i < 8;i++) xasm::database[addr + i] = temp._charval[i];
            }else{
                for(int i = 0;i < toWrite.length();i++){
                    xasm::database[addr + i] = toWrite[i];
                }
            }
            continue;
        }
        if(flags["log_level"] == "full" || flags["log_level"] == "normal") sendLog(flags["log_level"],"ast generated");
        xasm::asm_block temp_block = xasm::translateToASMStruct(ast);
        blocks.push_back(xasm::bytecode_block());
        blocks[blocks.size()-1].block_name = temp_block.block_name;
        if(flags["log_level"] == "full" || flags["log_level"] == "normal") sendLog(flags["log_level"],"transated to asm_struct");
        xasm::bytecode_block b_block = xasm::translateToBytecode(temp_block,blocks);
        if(flags["log_level"] == "full" || flags["log_level"] == "normal") sendLog(flags["log_level"],"generated bytecode block");
        blocks[blocks.size()-1] = b_block;
    }
    close(fd);
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
    cout << "[Xasm] Version " << __xlang_version << " build " << getBuildTime() << "\n";
    #ifdef __xlang_insider
    cout << "Insider Preview: This product is not for personal users. Many features are not complete in this version." << endl;
    #endif
    /*try{*/
        parse_args(argc,argv);
        for(auto i = flags.begin();i != flags.end();i++){
            cout << "arg:" << i->first << ":" << i->second << " ";
        }
        if(flags["terminal"] == "true") terminal();
        if(flags["log_level"] == "") flags["log_level"] = "normal";
        else if(flags["file"] != ""){
            std::cout << "\nlog level as " << flags["log_level"] << std::endl;
            with_file(flags["file"]);
            save_xmvef();
        }
        else throw compiler_error("unknown compiler mode",0,0);
    /*}catch(compiler_error e){
        cout << e.what();
    };*/
    //string s;
    
}