CXX := g++
CXXFLAG := -static -shard
CXXFLAGS := -static -shard

device_test: xtimevm/tests/devtest.cpp
	$(CXX) $(CXXFLAGS) xtimevm/tests/devtest.cpp -o device_test

install: device_test.exe
	mv device_test.exe ./build/

clean:
	rm -rf build/device_test*