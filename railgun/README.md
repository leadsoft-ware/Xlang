
# How to use?
first, open tests folder, execute `g++ ast_test.cpp -g -o ast_test`,then execute `./ast_test terminal`.

# Xlang 文法

```c
memberexpr ::= primary { "." primary }
indexof ::= memberexpr { arraysubscript }
arraysubscript ::= "[" primary "]"
term ::= indexof { ("*"|"/"|"%") indexof }
addexpr ::= term { ("+"|"-") term }
relexpr ::= addexpr { ("<" | ">" | "<=" | ">=") addexpr }
eqexpr ::= relexpr { ("=="|"!=") relexpr }
andexpr ::= eqexpr { "&&" eqexpr }
orexpr ::= andexpr { "||" andexpr }
rightexpr ::= indexof ( "=" | "+=" | "-=" | "*=" | "/=" | "%=" | ":" ) orexpr
            | orexpr
block ::= { statement ";" }
argument ::= rightexpr { "," rightexpr }
function_call_stmt ::= primary "(" argument ")"
function_def_stmt ::= "function" argument ""
if_stmt ::= "if" "(" rightexpr ")" "{" block "}" { "else" ( block | statement ) }
while_stmt ::= "while" "(" rightexpr ")" "{" block "}"
for_stmt ::= "for" "(" rightexpr ";" rightexpr ";" rightexpr ")" "{" block "}"
statement ::= ( if_stmt | while_stmt | for_stmt | rightexpr ) ";"
normal_stmt_return ::= "return" rightexpr
normal_stmt_var ::= "var" rightexpr { "," rightexpr }
normal_stmt_continue ::= "continue"
normal_stmt_break ::= "break"
```

# Xlang AST Sample

```json
{
 type: "leaf",
 match: "block",
 node: [
  {
   type: "leaf",
   match: "if_stmt",
   node: [
    {
     type: "leaf",
     match: "expression",
     node: [
      {
       type: "leaf",
       match: "expression",
       node: [
        {
         type: "child",
         match: "primary",
         token: "tok_int:1"

        },        {
         type: "child",
         match: "operator",
         token: "tok_add:+"

        },        {
         type: "child",
         match: "primary",
         token: "tok_int:1"

        },
       ]
      },      {
       type: "child",
       match: "operator",
       token: "tok_equal:=="

      },      {
       type: "child",
       match: "primary",
       token: "tok_int:2"

      },
     ]
    },    {
     type: "leaf",
     match: "block",
     node: [

     ]
    },
   ]
  },
 ]
}
```



XlangIR中间码表现形式
```c++
import "@packages/vmlib/libxlang.xasm":xasm;
import "@packages/stdlib/stdlib.xlang":xlang;

// 不使用相同字节码函数的引用
vmbyte* main = function(norefernce) int (argc:int,argv:char**){
    int i = 0;
    int j = 1;
    i = i + j + 1;
    i *= i;
    return i;
};

```
```c++
load("@packages/vmlib/libxlang.xlang"); // 伪语句
load("@packages/stdlib/stdlib.xlang");
struct( s , a:int , b:int );
f"main"{
    var i,8;
    var j,8;
    i = 0;
    j = 1;
    i = i + j;
    i = i + 1;
    i = i * i;
    ret i;
}

entry{
    var main,8;
    main = f"main";
    var ex9123jsado,8;
    exec main,argc,argv,ex9123jsado;
}

```