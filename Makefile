# ------------------------------------------
# SyntaxSculptor
# 
# SPDX-License-Identifier: MIT
# ------------------------------------------
 
.PHONY: all clean proc_monitor

CPP_COMPILER := g++
CPP_FLAGS := -Wall -Wextra -std=c++20 -Wpedantic -Werror
SRC := src/main.cpp
OUT := build/proc_monitor

all: proc_monitor

proc_monitor:
	mkdir -p build
	$(CPP_COMPILER) $(CPP_FLAGS) $(SRC) -o $(OUT)

clean:
	rm -rf build/