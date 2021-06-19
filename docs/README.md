 ### Language [zh_cn](README.md) | [english](README_en-US.md)





### Xlang中文文档



**目前版本支持状态**

- [ ] Xscript (All version)

- [ ] Xlang CLANNAD (alpha)

- [x] Xlang Railgun (beta)



### 关于Xlang



Xlang是一个编译型的小型语言，使用Anti-996开源协议，使用与其配套的XtimeVM(*未完成*)来运行字节码

**此项目目前没有任何实际用途，不推荐用于实际开发，仅供学习参考使用！**



### Xlang文法：



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
function_def_stmt ::= "function" primary "(" argument ")" "{" block "}"
if_stmt ::= "if" "(" rightexpr ")" "{" block "}" { "else" ( block | statement ) }
while_stmt ::= "while" "(" rightexpr ")" "{" block "}"
for_stmt ::= "for" "(" rightexpr ";" rightexpr ";" rightexpr ")" "{" block "}"
statement ::= ( if_stmt | while_stmt | for_stmt | rightexpr ) ";"
normal_stmt_return ::= "return" rightexpr
normal_stmt_var ::= "var" rightexpr { "," rightexpr }
normal_stmt_continue ::= "continue"
normal_stmt_break ::= "break"
```



### Xasm基本语法

```c
asm_primary ::= primary
asm_primary_pointer ::= "*" asm_primary
asm_argument ::= asm_primary { "," asm_primary }
asm_statement ::= tok_id "(" asm_argument ")"
asm_block ::= tok_id ":" "{" asm_statement ";" { asm_statement ";" }
```

