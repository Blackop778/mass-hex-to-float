CXX = g++

hex2float:
	$(CXX) -o hex2float hex2float.cpp
clean:
	rm -f hex2float
