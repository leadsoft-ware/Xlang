CXX := g++
CXXFLAG := -ldl -fPIC -shared
CXXFLAGS := -ldl -fPIC -shared

device_test: xtimevm/tests/devtest.cpp
	$(CXX) $(CXXFLAGS) xtimevm/tests/devtest.cpp -o device_test

install: device_test
	mv device_test ./build/

clean:
	rm -rf build/device_test*