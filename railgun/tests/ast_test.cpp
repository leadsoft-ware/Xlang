#include "../include/xast.cpp"
using namespace std;

int main(){
    cout << "[Xlang Test Program] xast&parser\n";
    string s;
    while(s != "!exit"){
        cout << "(input)>";
        getline(cin,s);
        if(s=="exit") return 0;
        try{
            Lexer lexer(s);
            Token tok = lexer.getNextToken();
            while(tok.tok_val != tok_eof){
                cout << tok.toString() << " ";
                tok=lexer.getNextToken();
            }
            cout << endl;
            lexer.pos = 0;lexer.col = 0;lexer.line = 0;
            lexer.getNextToken();
            xast::astree ast = xast::rule_parser::rightexpr_parser(&lexer).match();
            cout << ast.matchWithRule << endl;
        }catch(compiler_error e){
            std::cerr << e.what() << std::endl;
        };
    }
}