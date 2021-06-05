# Xlang 文法
# Xlang Basic Syntax


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
rightexpr ::= indexof "=" orexpr
            | orexpr
block ::= { statement ";" }
if_stmt ::= "if" "(" rightexpr ")" "{" block "}" { "else" ( block | statement ) }
while_stmt ::= "while" "(" rightexpr ")" "{" block "}"
for_stmt ::= "for" "(" rightexpr ";" rightexpr ";" rightexpr ")" "{" block "}"
statement ::= ( if_stmt | while_stmt | for_stmt | rightexpr ) ";"
normal_stmt_return ::= "return" rightexpr
normal_stmt_var ::= "var" rightexpr { "," rightexpr }
normal_stmt_continue ::= "continue"
normal_stmt_break ::= "break"
```

