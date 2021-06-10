### Language [zh_cn](README.md) | [english](README_en-us.md)





### Xlang English document



**Xlang Version Support Status**

- [ ] Xscript (All version)

- [ ] Xlang CLANNAD (alpha)

- [x] Xlang Railgun (beta)



### About Xlang



Xlang is a compiled programming language,open source with GPL agreement,using XtimeVM to run bytecode called Xasm(*Not finish yet*).

**Not recommended to actual development, it's only for reference use learning!**



### Xlang grammar：



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



### Xasm basic grammar

```c
asm_primary ::= primary
asm_primary_pointer ::= "*" asm_primary
asm_argument ::= asm_primary { "," asm_primary }
asm_statement ::= tok_id "(" asm_argument ")"
asm_block ::= tok_id ":" "{" asm_statement ";" { asm_statement ";" }
```

