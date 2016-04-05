
CC=g++
CFLAGS=-std=c++0x -Wall

BUILDDIR	= build
SRCDIR		= src
TESTDIR		= test

SRCFILES  = $(wildcard $(SRCDIR)/*.cpp)
OBJFILES  = $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SRCFILES))
DEPSFILES = $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.depends,$(SRCFILES))
TESTFILES = $(basename $(wildcard $(TESTDIR)/*.cpp))

all: $(TESTFILES)

dir:
	mkdir -p $(BUILDDIR)

test: test_ndarray test_refcounter
	./test_refcounter
	./test_ndarray

test_ndarray: $(OBJFILES)
	$(CC) -I$(SRCDIR) $(CFLAGS) $(OBJFILES) test_ndarray.cpp -o test_ndarray

test_refcounter: $(OBJFILES)
	$(CC) -I$(SRCDIR) $(CFLAGS) $(OBJFILES) test_refcounter.cpp -o test_refcounter

$(OBJFILES): $(BUILDDIR)/%.o : $(SRCDIR)/%.cpp
	$(CC) -c $(CFLAGS) $< -o $@


# %.cpp:
# 	$(CC) $(CFLAGS) -c $< -o $@

# $(TESTFILES): $(TESTDIR)/%.cpp
# 	$(CC) -I$(SRCDIR) $(CFLAGS) $(OBJFILES) $< -o $@


%.depends: %.cpp
	$(CC) -M $(CFLAGS) $< > $@

clean:
	$(RM) $(OBJFILES) $(DEPSFILES) test_ndarray test_refcounter

print-%:
	@echo '$*=$($*)'

-include $(DEPSFILES)

# test: test_refcounter test_ndarray
# 	./test_refcounter
# 	./test_ndarray

# test_refcounter: refcounter.cpp test_refcounter.cpp
# 	$(CC) $(CFLAGS) refcounter.cpp test_refcounter.cpp -o test_refcounter

# test_ndarray: refcounter.cpp exceptions.cpp ndarray.cpp test_ndarray.cpp
# 	$(CC) $(CFLAGS) refcounter.cpp exceptions.cpp ndarray.cpp test_ndarray.cpp -o test_ndarray

# clean:
# 	$(RM) test_refcounter
# 	$(RM) test_ndarray 

