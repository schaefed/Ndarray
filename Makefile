
CC=g++
CFLAGS=-std=c++0x -Wall -g -pedantic

BUILDDIR  = build
SRCDIR	  = src
TESTDIR	  = test

SRCFILES  = $(wildcard $(SRCDIR)/*.cpp)
OBJFILES  = $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SRCFILES))
DEPSFILES = $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.depends,$(SRCFILES))
TESTFILES = $(wildcard $(TESTDIR)/*.cpp)
TESTS     = $(basename $(TESTFILES))

all: test

test: $(TESTS)
	$(TESTDIR)/test_refcounter
	$(TESTDIR)/test_ndarray

$(OBJFILES): $(BUILDDIR)/%.o : $(SRCDIR)/%.cpp
	$(CC) -c $(CFLAGS) $< -o $@

# not correct!
$(TESTS): $(OBJFILES) $(TESTFILES)
	$(CC) -I$(SRCDIR) $(CFLAGS) $(OBJFILES) $< -o $@

%.depends: %.cpp
	$(CC) -M $(CFLAGS) $< > $@

dir:
	mkdir -p $(BUILDDIR)

clean:
	$(RM) $(OBJFILES) $(DEPSFILES) $(TESTS) 

print-%:
	@echo '$*=$($*)'

-include $(DEPSFILES)



