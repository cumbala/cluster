TARGET=cluster
COMPILER=gcc
COMPILER_OPTIONS=-std=c99 -Wall -Wextra -Werror -DNDEBUG
VALGRIND=valgrind
VALGRIND_OPTIONS=--leak-check=full --leak-resolution=high

default: all

test: all
	python3 ./test.py $(TARGET) --valgrind

valgrind: all
	$(VALGRIND) $(VALGRIND_OPTIONS) ./$(TARGET) objekty 8

all:
	$(COMPILER) $(COMPILER_OPTIONS) $(TARGET).c -o $(TARGET) -lm