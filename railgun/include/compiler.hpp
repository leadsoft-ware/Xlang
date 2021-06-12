#include "../xasm/include/xasm.cpp"
#include <map>


namespace xasm{

    xasm::asm_block visit_ast(xast::astree ast){
        if(ast.matchWithRule == "normal_stmt_var"){
            for(int i = 0;i < ast.node.size();i++){
                
            }
        }
    }

    // 翻译到asm结构
    class xlang_compiler{
        std::vector<xasm::asm_block> result;
        xast::astree ast;
        std::map<std::string,std::string> flag; // 编译参数
        public:
        xast::astree &get_astree(){return ast;}
        std::vector<xasm::asm_block> &get_result(){return result;}
        void compile(){
            
        }
        xlang_compiler(xast::astree ast,std::map<std::string,std::string> map){
            this->ast = ast;
            this->flag = map;
        }
    };
};