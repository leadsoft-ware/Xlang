CXX := g++
CXXFLAG := -g -ldl
CXXFLAGS := -g

Xasm: xasm/tests/xasm_test.cpp xasm/include/*
	$(CXX) xasm/tests/xasm_test.cpp $(CXXFLAG) -o xasm/tests/xasm

Xlang: tests/ast_test.cpp include/*
	$(CXX) tests/ast_test.cpp $(CXXFLAG) -o tests/xlang

Xmvefdump: xmvefdump/objdump.cpp xasm/include/*
	$(CXX) xmvefdump/objdump.cpp $(CXXFLAG) -o xmvefdump/objdump

XtimeVM: xtimevm/tests/xtime_test.cpp xtimevm/include/*
	$(CXX) xtimevm/tests/xtime_test.cpp $(CXXFLAG) -o xtimevm/xtimevm

refresh: refresh_version
	./refresh_version fast

devices: 
	make -f Devices.Makefile install

install: clean Xasm Xlang Xmvefdump XtimeVM devices
	mv xasm/tests/xasm build/
	mv tests/xlang build/
	mv xmvefdump/objdump build/
	mv xtimevm/xtimevm build/

quick: refresh install

clean: 
	make -f Devices.Makefile clean
	rm -rf ./build/xasm* ./build/xlang* ./build/objdump* ./build/xtime* ./refresh_version