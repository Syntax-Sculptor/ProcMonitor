# ------------------------------------------
# SyntaxSculptor
#
# SPDX-License-Identifier: MIT
# ------------------------------------------

.PHONY: all clean proc_monitor test

CPP_COMPILER := g++
CPP_FLAGS := -Wall -Wextra -std=c++20 -Wpedantic -Werror
INCLUDES := -Ilib/ -Isrc/

LIB_SRC := src/stat_parser.cpp src/proc_monitor.cpp
SRC     := src/main.cpp $(LIB_SRC)
OUT     := build/proc_monitor

TEST_SRC := tests/test_main.cpp tests/stat_parser_test.cpp tests/proc_monitor_test.cpp
TEST_OUT := build/run_tests

CPPUTEST_HOME     := ext/cpputest
CPPUTEST_INCLUDES := -isystem $(CPPUTEST_HOME)/include
CPPUTEST_LIB      := $(CPPUTEST_HOME)/build/src/CppUTest/libCppUTest.a

all: proc_monitor

proc_monitor:
	mkdir -p build
	$(CPP_COMPILER) $(CPP_FLAGS) $(INCLUDES) $(SRC) -o $(OUT)

test: $(CPPUTEST_LIB)
	mkdir -p build
	$(CPP_COMPILER) $(CPP_FLAGS) $(INCLUDES) $(CPPUTEST_INCLUDES) \
	    $(LIB_SRC) $(TEST_SRC) $(CPPUTEST_LIB) -o $(TEST_OUT)
	./$(TEST_OUT) -v

$(CPPUTEST_LIB):
	git submodule update --init --recursive
	cmake -S $(CPPUTEST_HOME) -B $(CPPUTEST_HOME)/build -DTESTS=OFF
	cmake --build $(CPPUTEST_HOME)/build -j

clean:
	rm -rf build/