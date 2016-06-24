all: libsuffix.so suffix_test

libsuffix.so: suffix.cpp Makefile suffix.hpp
	g++ -std=c++11 -g -o libsuffix.so -fPIC -shared suffix.cpp
suffix_test: suffix_test.cpp libsuffix.so
	g++ -std=c++11 -g -o suffix_test -lsuffix -L. suffix_test.cpp
clean:
	rm -f core libsuffix.so suffix_test *.o
