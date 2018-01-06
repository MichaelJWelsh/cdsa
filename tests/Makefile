C_COMPILER=gcc
C_FLAGS=-Wall -Wextra -Werror -pedantic-errors -std=c89

CPP_COMPILER=g++
CPP_FLAGS=-Wall -Wextra -Werror -pedantic-errors -std=c++11

all: test_list test_rbtree

test_list:
	$(C_COMPILER) test_list.c ../src/list.c -o test_list $(C_FLAGS)
	./test_list C89
	rm -f test_list
	$(CPP_COMPILER) test_list.c ../src/list.c -o test_list $(CPP_FLAGS)
	./test_list C++11
	rm -f test_list

test_rbtree:
	$(C_COMPILER) test_rbtree.c ../src/rbtree.c -o test_rbtree $(C_FLAGS)
	./test_rbtree C89
	rm -f test_rbtree
	$(CPP_COMPILER) test_rbtree.c ../src/rbtree.c -o test_rbtree $(CPP_FLAGS)
	./test_rbtree C++11
	rm -f test_rbtree
