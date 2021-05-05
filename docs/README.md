### Language [README.md](README.md) | [README_zh-cn.md](README_zh-cn.md)
### Update logs [updates.log](updates.log)
# Tips
对于在大陆的用户的快速克隆地址: `https://hub.fastgit.org/leadsoft-ware/Xlang`<br/>
A fast clone url for China Mainland Users: `https://hub.fastgit.org/leadsoft-ware/Xlang`<br/>
**We're no longer support windows platform in future.**<br/>
**If you want to run it on windows, you must replace all the `long int` and `long` to `long long`,this is a bug of mingw.**<br/>
**我们不再支持Windows平台**<br/>
**如果你想要继续在Windows平台上运行，你必须把所有的`long int`和`long`替换成`long long`，这是mingw的一个bug**

# Get Started
## DOWNLOAD AND COMPILE IT
- first,download this repository to your computer.and chdir to it.
- if you don't have g++ compiler?please setting up beforce you compile it
- if you finish this step.then execute it:
`g++ ./xlang.cpp -o xlang -g`
`g++ ./vm/vm.cpp -o ./vm/vm -g`
you also can don't append `-g` this arg , that is for debug

- If you're using macOS,bits/stdc++.h is missing.you can install brew and use g++-10 to compile it.
## Basic Syntax
### Function declaration
```go
func [return type] [function name](arg0_type:arg0_name,...){
  code here
};

sample here:
func int main(int:argc,ptr_char:argv){
  return 0;
};
```
### Variable declaration
```go
[typename] varname<=[any]>,...;

sample here:
int i=0;
```
### Description:
just like:
```c
// something
```
multi line:
```c
/*
something here
*/
```

### Expression And Statement:
**ATTENTION TO**
- There are no brackets in the expression to modify the priority. The priority is sorted according to four operations. The logical symbol is calculated after the operation, and the addition, subtraction, multiplication and division have priority. In addition, the member expression (.) and the type expression (:) are used for structure and function declaration respectively

- All the expression and statement must has a **SEMICOLON** after it.

### If Statement And Flow control statement

Syntax of If Statement:
```c++
if(expr){
  somthing here
}elif{
  something here
}else{
  same
};
```

Syntax Of flow control statement:
```c++
for(expr0,expr1,expr2){};
while(expr){};
```

### How to import a file
```go
import(headers:"hahaha.xlang",...);
```

### First Program
**Basic Compiler doesn't have crt we'll fix it later(or never fix it : -)**<br>
open an new file and type something in this file:
```go
ptr_char p="Hello,World";
```
If no bugs here,it will print:
```bash
ubuntu@VM-0-16-ubuntu:~/Xlang$ ./vm/vm
Xtime VM Core[1.0.01]
Starting...
mov reg0,0;
mov_m [11],reg0,8;
Memory Watcher=>0
 int val=>8022930118255863112
 Charter Val=>Hello,Wo
ubuntu@VM-0-16-ubuntu:~/Xlang$ 
```