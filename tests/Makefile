C_COMPILER=gcc
C_FLAGS=-Wall -Werror -pedantic-errors -std=c89

CPP_COMPILER=g++
CPP_FLAGS=-Wall -Werror -pedantic-errors -std=c++11

all: test_list

test_list:
	$(C_COMPILER) test_list.c ../src/list.c -o test_list $(C_FLAGS)
	./test_list C89
	rm -f test_list
	$(CPP_COMPILER) test_list.c ../src/list.c -o test_list $(CPP_FLAGS)
	./test_list C++11
	rm -f test_list
