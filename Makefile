
CC=g++
CFLAGS=-std=c++0x -Wall

SRCDIR=.

test: test_refcounter test_ndarray
	./test_refcounter
	./test_ndarray

test_refcounter: refcounter.cpp test_refcounter.cpp
	$(CC) $(CFLAGS) refcounter.cpp test_refcounter.cpp -o test_refcounter

test_ndarray: refcounter.cpp exceptions.cpp ndarray.cpp test_ndarray.cpp
	$(CC) $(CFLAGS) refcounter.cpp exceptions.cpp ndarray.cpp test_ndarray.cpp -o test_ndarray

clean:
	$(RM) test_refcounter
	$(RM) test_ndarray 

