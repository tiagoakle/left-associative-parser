UNAME_S := $(shell uname -s)

#include for testing 
TEST_INCLUDE = -I./external/gmock-1.7.0/gtest/include -I./external/gmock-1.7.0/include
INCLUDE = -I./include $(TEST_INCLUDE)

#Testing libraries
GTEST_DIR=./external/gmock-1.7.0/gtest
GMOCK_DIR=./external/gmock-1.7.0/

AR=ar -rv

CPP_FILES := $(wildcard src/*.cpp) 
#remove src/main.cpp 
CPP_FILES := $(filter-out src/main.cpp, $(CPP_FILES))
TEST_CPP_FILES := $(wildcard test/*.cpp) 
OBJ_FILES := $(addprefix bin/,$(notdir $(CPP_FILES:.cpp=.o)))
TEST_OBJ_FILES := $(addprefix testbin/,$(notdir $(TEST_CPP_FILES:.cpp=.o)))

ifeq ($(UNAME_S),Linux)
    CPP = g++
    CFLAGS = -std=c++11 -Wall -Wextra 
	EXE_NAME = _linux
endif

ifeq ($(UNAME_S),Darwin)
    CPP = clang++
    CFLAGS = -std=c++11 -stdlib=libc++ -Wall -Wextra -w -g
	EXE_NAME = _darwin
endif

all: main

#We treat main differently to all the other files so that when we build the test suite 
# we don't link two files with a main symbol
main.o:	
	$(CPP) $(CFLAGS) $(INCLUDE) $(TEST_INCLUDE) -c ./src/main.cpp -o ./bin/main.o

main: $(OBJ_FILES) main.o
	$(CPP) -stdlib=libc++ -std=c++11 $(OBJ_FILES) ./bin/main.o -o ./bin/main$(EXE_NAME).exe

#compile gtest and gmock
bin/libgmock.a: 
	$(CPP) $(CFLAGS) -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
           -isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} \
           -pthread -c ${GTEST_DIR}/src/gtest_main.cc -o ./bin/gtest_main.o
	$(CPP) $(CFLAGS) -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
           -isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} \
           -pthread -c ${GTEST_DIR}/src/gtest-all.cc -o ./bin/gtest-all.o           
	$(CPP) $(CFLAGS) -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
      	   -isystem ${GMOCK_DIR}/include -I${GMOCK_DIR} \
           -pthread -c ${GMOCK_DIR}/src/gmock-all.cc -o ./bin/gmock-all.o
	$(AR) ./bin/libgmock.a ./bin/gtest_main.o ./bin/gtest-all.o ./bin/gmock-all.o 

#Compile all test objects
testbin/%.o: test/%.cpp
	$(CPP) $(CFLAGS) $(INCLUDE) $(TEST_INCLUDE) -c $< -o $@

#Compile all algorithm objects
bin/%.o: src/%.cpp 	
	$(CPP) $(CFLAGS) $(INCLUDE) $(TEST_INCLUDE) -c $< -o $@

test: $(TEST_OBJ_FILES) $(OBJ_FILES) bin/libgmock.a
	mkdir -p bin
	mkdir -p testbin 
	$(CPP) $(CFLAGS) $(TEST_OBJ_FILES) $(OBJ_FILES) bin/libgmock.a -o ./bin/unittest.exe

clean:
	rm ./bin/*; rm ./testbin/*
