
CC=g++

CFLAGS=-std=c++0x -Wall

test: test_refcounter test_ndarray
	./test_refcounter
	./test_ndarray

test_refcounter: refcounter.hpp test_refcounter.cpp
	$(CC) $(CFLAGS) refcounter.hpp test_refcounter.cpp -o test_refcounter

test_ndarray: refcounter.hpp exceptions.hpp ndarray.hpp test_ndarray.cpp
	$(CC) $(CFLAGS) refcounter.hpp exceptions.hpp ndarray.hpp test_ndarray.cpp -o test_ndarray

clean:
	$(RM) test_refcounter
	$(RM) test_ndarray 

