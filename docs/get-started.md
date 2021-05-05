# Get Started
- *Well, if you want to test the compiler and compile your source code,you should read this.*

### Setup your Xlang Compiler
- *In the Xlang,default compiler is xl*<br>

Everyone knows Xlang is base on C++,so you must have a C++ compiler on your computer.

Compile the xlang compiler and the VM
```bash
cd {replaceYourXlangSourceCodePathHere}
make xl && make debug_release
```

### compile your code
If you already setup your compiler,then you can compile your code now.

- *please read the README beforce you write your code*

you can start learn xlang from test.xlang.

compile your code:
```
./xl input_filename=\"{your filename here}\" output_filename=\"{output filename}.xvm\"
```
