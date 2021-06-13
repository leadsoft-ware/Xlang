CXX := g++
CXXFLAG := -g

Xasm: xasm/tests/xasm_test.cpp xasm/include/*
	$(CXX) xasm/tests/xasm_test.cpp $(CXXFLAG) -o xasm/tests/xasm

Xlang: tests/ast_test.cpp include/*
	$(CXX) tests/ast_test.cpp $(CXXFLAG) -o tests/xlang

install: Xasm Xlang
	mv xasm/tests/xasm build/
	mv tests/xlang build/

clean: 
	rm -rf ./build/xasm* ./build/xlang*