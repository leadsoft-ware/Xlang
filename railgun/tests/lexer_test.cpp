#include "../include/lexer.cpp"
using namespace std;

int main(){
    cout << "[Xlang Test Program] Lexer\n";
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
        }catch(compiler_error e){
            std::cerr << e.what() << std::endl;
        };
    }
}