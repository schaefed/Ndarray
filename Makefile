
CC=g++
CFLAGS=-std=c++0x -Wall -g -pedantic

BUILDDIR  = build
SRCDIR	  = src
TESTDIR	  = test

# SRCFILES  = $(wildcard $(SRCDIR)/*.cpp)
# OBJFILES  = $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(SRCFILES))
# DEPSFILES = $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.depends, $(SRCFILES))
TESTFILES = $(wildcard $(TESTDIR)/*.cpp)
TESTS  = $(patsubst $(TESTDIR)/%.cpp, $(TESTDIR)/%.test, $(TESTFILES))


all: clean test

test: $(TESTS)
	$(foreach test,$(TESTS),./$(test);)


$(TESTS): $(OBJFILES) $(TESTDIR)/%.test : $(TESTDIR)/%.cpp
	$(CC) -I$(SRCDIR) $(CFLAGS) $< -o $@


# $(OBJFILES): dir $(BUILDDIR)/%.o : $(SRCDIR)/%.cpp
# 	$(CC) -c $(CFLAGS) $< -o $@

# %.depends: %.cpp
# 	$(CC) -M $(CFLAGS) $< > $@

# dir:
# 	mkdir -p $(BUILDDIR)

clean:
	$(RM) $(OBJFILES) $(DEPSFILES) $(TESTS) 

print-%:
	@echo '$*=$($*)'

-include $(DEPSFILES)



