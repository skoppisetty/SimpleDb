CC = g++ -O2 -g -Wno-deprecated -Wno-write-strings 
LDFLAGS := -pthread -L./
tag = -i

BIN=./bin/
SOURCE=./source/

ifdef linux
tag = -n
endif


# A Makefile for fusing Google Test and building a sample test against it.
#
# SYNOPSIS:
#
#   make [all]  - makes everything.
#   make TARGET - makes the given target.
#   make check  - makes everything and runs the built sample test.
#   make clean  - removes all files generated by make.

# Points to the root of fused Google Test, relative to where this file is.
FUSED_GTEST_DIR = source/fused-src

# Paths to the fused gtest files.
FUSED_GTEST_H = $(FUSED_GTEST_DIR)/gtest/gtest.h
FUSED_GTEST_ALL_CC = $(FUSED_GTEST_DIR)/gtest/gtest-all.cc

# Where to find gtest_main.cc.
GTEST_MAIN_CC = source/src/gtest_main.cc

# Flags passed to the preprocessor.
# We have no idea here whether pthreads is available in the system, so
# disable its use.
CPPFLAGS += -I$(FUSED_GTEST_DIR) -DGTEST_HAS_PTHREAD=0

# Flags passed to the C++ compiler.
CXXFLAGS += -g

$(FUSED_GTEST_H) :
	source/scripts/fuse_gtest_files.py $(FUSED_GTEST_DIR)

$(FUSED_GTEST_ALL_CC) :
	source/scripts/fuse_gtest_files.py $(FUSED_GTEST_DIR)

gtest-all.o : $(FUSED_GTEST_H) $(FUSED_GTEST_ALL_CC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(FUSED_GTEST_DIR)/gtest/gtest-all.cc -o $(BIN)$@

gtest_main.o : $(FUSED_GTEST_H) $(GTEST_MAIN_CC)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(GTEST_MAIN_CC) -o $(BIN)$@


# make all
# generates main gtesting.o and test scripts
all: main gtesting.o test1 test2

# make test
# generates only the test script - please do make all
test1: Record.o Comparison.o ComparisonEngine.o Schema.o File.o  BigQ.o Pipe.o DBFile.o HeapFile.o SortedFile.o y.tab.o lex.yy.o test1.o 
	$(CC) -o $(BIN)test1 $(BIN)Record.o $(BIN)Comparison.o $(BIN)ComparisonEngine.o $(BIN)Schema.o $(BIN)File.o  $(BIN)BigQ.o $(BIN)Pipe.o  $(BIN)HeapFile.o $(BIN)SortedFile.o $(BIN)DBFile.o $(BIN)y.tab.o $(BIN)lex.yy.o $(BIN)test1.o -lfl $(LDFLAGS)
	
test2: Record.o Comparison.o ComparisonEngine.o Schema.o File.o  BigQ.o Pipe.o DBFile.o HeapFile.o SortedFile.o y.tab.o lex.yy.o test2.o 
	$(CC) -o $(BIN)test2 $(BIN)Record.o $(BIN)Comparison.o $(BIN)ComparisonEngine.o $(BIN)Schema.o $(BIN)File.o  $(BIN)BigQ.o $(BIN)Pipe.o  $(BIN)HeapFile.o $(BIN)SortedFile.o $(BIN)DBFile.o $(BIN)y.tab.o $(BIN)lex.yy.o $(BIN)test2.o -lfl $(LDFLAGS)

main: Record.o Comparison.o ComparisonEngine.o Schema.o File.o y.tab.o lex.yy.o  BigQ.o Pipe.o DBFile.o HeapFile.o SortedFile.o main.o
	$(CC) -o $(BIN)main $(BIN)Record.o $(BIN)Comparison.o $(BIN)ComparisonEngine.o $(BIN)Schema.o $(BIN)File.o $(BIN)y.tab.o $(BIN)lex.yy.o $(BIN)BigQ.o $(BIN)Pipe.o $(BIN)HeapFile.o $(BIN)SortedFile.o $(BIN)DBFile.o $(BIN)main.o -lfl $(LDFLAGS)

gtesting.o: gtest-all.o gtest_main.o BigQ.o Pipe.o
	$(CC) $(BIN)Record.o $(BIN)Comparison.o $(BIN)ComparisonEngine.o $(BIN)Schema.o $(BIN)File.o $(BIN)y.tab.o $(BIN)lex.yy.o $(BIN)HeapFile.o $(BIN)SortedFile.o $(BIN)BigQ.o $(BIN)Pipe.o $(BIN)DBFile.o $(BIN)gtest_main.o $(BIN)gtest-all.o $(SOURCE)gtesting.cc -o $(BIN)$@ $(LDFLAGS)
	
test1.o: $(SOURCE)test1.cc
	$(CC) -g -c  $< -o $(BIN)$@ $(LDFLAGS)

test2.o: $(SOURCE)test2.cc
	$(CC) -g -c  $< -o $(BIN)$@ $(LDFLAGS)

main.o: $(SOURCE)main.cc
	$(CC) -g -c  $< -o $(BIN)$@
 	
Comparison.o: $(SOURCE)Comparison.cc
	$(CC) -g -c  $< -o $(BIN)$@
	
ComparisonEngine.o: $(SOURCE)ComparisonEngine.cc
	$(CC) -g -c  $< -o $(BIN)$@
	
HeapFile.o: $(SOURCE)HeapFile.cc
	$(CC) -g -c  $< -o $(BIN)$@

SortedFile.o: $(SOURCE)SortedFile.cc
	$(CC) -g -c  $< -o $(BIN)$@

DBFile.o: $(SOURCE)DBFile.cc
	$(CC) -g -c  $< -o $(BIN)$@

File.o: $(SOURCE)File.cc
	$(CC) -g -c  $< -o $(BIN)$@

Record.o: $(SOURCE)Record.cc
	$(CC) -g -c  $< -o $(BIN)$@

Pipe.o: $(SOURCE)Pipe.cc
	$(CC) -g -c  $< -o $(BIN)$@ $(LDFLAGS)

BigQ.o: $(SOURCE)BigQ.cc
	$(CC) -g -c  $< -o $(BIN)$@ $(LDFLAGS)

Schema.o: $(SOURCE)Schema.cc
	$(CC) -g -c  $< -o $(BIN)$@
	
y.tab.o: $(SOURCE)Parser.y
	yacc -d $(SOURCE)Parser.y
	mv y.tab.* $(SOURCE)
	sed $(tag) $(SOURCE)y.tab.c -e "s/  __attribute__ ((__unused__))$$/# ifndef __cplusplus\n  __attribute__ ((__unused__));\n# endif/" 
	g++ -c $(SOURCE)y.tab.c -o $(BIN)$@

lex.yy.o: $(SOURCE)Lexer.l
	lex  $(SOURCE)Lexer.l
	mv lex.* $(SOURCE)
	gcc  -c $(SOURCE)lex.yy.c -o $(BIN)$@

clean: 
	rm -f $(BIN)*.o
	rm -f $(BIN)*.out
	rm -f $(BIN)y.tab.c
	rm -f $(BIN)lex.yy.c
	rm -f $(BIN)y.tab.h
	rm -f $(BIN)*.bin
	rm -f $(BIN)main $(BIN)test
	rm -f ./DATA/10M/*.metadata




