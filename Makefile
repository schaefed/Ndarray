
CC=g++
CFLAGS=-std=c++14 -Wall -g -pedantic

BUILDDIR  = build
SRCDIR	  = src
TESTDIR	  = test

TESTFILES = $(wildcard $(TESTDIR)/*.cpp)
TESTS  = $(patsubst $(TESTDIR)/%.cpp, $(TESTDIR)/%.test, $(TESTFILES))


all: clean test

test: $(TESTS)
	$(foreach test,$(TESTS),./$(test);)


$(TESTS): $(OBJFILES) $(TESTDIR)/%.test : $(TESTDIR)/%.cpp
	$(CC) -I$(SRCDIR) $(CFLAGS) $< -o $@


clean:
	$(RM) $(OBJFILES) $(DEPSFILES) $(TESTS) 

print-%:
	@echo '$*=$($*)'

-include $(DEPSFILES)



