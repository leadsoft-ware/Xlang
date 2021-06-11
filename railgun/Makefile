CXX := g++
CXXFLAG := -g

xasm: xasm/tests/xasm_test.cpp xasm/include/*
	$(CXX) xasm/tests/xasm_test.cpp $(CXXFLAG) -o xasm/tests/xasm

xlang: tests/ast_test.cpp include/*
	$(CXX) tests/ast_test.cpp $(CXXFLAG) -o tests/xlang

install: xasm xlang
	mv xasm/tests/xasm build/
	mv tests/xlang build/