all: libsuffix.so suffix_test ls_test eagle_test

libsuffix.so: suffix.cpp Makefile suffix.hpp suffix.tcc
	g++ -std=c++11 -g -Wall -o libsuffix.so -fPIC -shared suffix.cpp
suffix_test: suffix_test.cpp libsuffix.so
	g++ -std=c++11 -g -Wall -o suffix_test suffix_test.cpp -lsuffix -L.
ls_test: ls.cpp libsuffix.so
	g++ -std=c++11 -g -Wall -o ls_test ls.cpp -lsuffix -L.
eagle_test: eagle_test.cpp libsuffix.so
	g++ -std=c++11 -g -Wall -o eagle_test eagle_test.cpp -lsuffix -L.
clean:
	rm -f core libsuffix.so suffix_test *.o
