# Makefile for Xlang compiler and Xtime VM
CXX := g++-10 # for my mac, you can set it to g++
CXXFLAGS = -std=c++2a -g -ldl

compiler: 
	$(CXX) xlang.cpp -o xlang $(CXXFLAGS)

test_lex: test_lex.cpp

vm: 
	$(CXX) vm/vm.cpp -o ./vm/vm $(CXXFLAGS)

debug_release:
	$(CXX)  ./xlang.cpp -o xlang	-g -ldl
	$(CXX) ./vm/vm.cpp -o ./vm/vm	-g -ldl

all: compiler vm

lib: lib/vmlib@latest/native
	cd lib/vmlib@latest/native && make all

run: all
	./xlang && ./vm/vm

clean:
	rm -rf test_lex.dSYM test_lex xlang.dSYM test.dSYM xlang test_lex vm/vm
	cd lib/vmlib@latest/native && make clean
