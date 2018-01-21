C_COMPILER=gcc
C_FLAGS=-Wall -Wextra -Werror -pedantic-errors -std=c89
C_GNU_FLAGS=-Wall -Wextra -Werror -std=gnu89

CPP_COMPILER=g++
CPP_FLAGS=-Wall -Wextra -Werror -pedantic-errors -std=c++11
CPP_GNU_FLAGS=-Wall -Wextra -Werror -std=gnu++11

all: test_list test_rbtree test_hashtable test_hash_string test_stack test_queue

test_list:
	$(C_COMPILER) test_list.c ../src/list.c -o test_list $(C_FLAGS)
	./test_list C89
	rm -f test_list
	$(C_COMPILER) test_list.c ../src/list.c -o test_list $(C_GNU_FLAGS)
	./test_list GNU89
	rm -f test_list
	$(CPP_COMPILER) test_list.c ../src/list.c -o test_list $(CPP_FLAGS)
	./test_list C++11
	rm -f test_list
	$(CPP_COMPILER) test_list.c ../src/list.c -o test_list $(CPP_GNU_FLAGS)
	./test_list GNU++11
	rm -f test_list

test_rbtree:
	$(C_COMPILER) test_rbtree.c ../src/rbtree.c -o test_rbtree $(C_FLAGS)
	./test_rbtree C89
	rm -f test_rbtree
	$(C_COMPILER) test_rbtree.c ../src/rbtree.c -o test_rbtree $(C_GNU_FLAGS)
	./test_rbtree GNU89
	rm -f test_rbtree
	$(CPP_COMPILER) test_rbtree.c ../src/rbtree.c -o test_rbtree $(CPP_FLAGS)
	./test_rbtree C++11
	rm -f test_rbtree
	$(CPP_COMPILER) test_rbtree.c ../src/rbtree.c -o test_rbtree $(CPP_GNU_FLAGS)
	./test_rbtree GNU++11
	rm -f test_rbtree

test_hashtable:
	$(C_COMPILER) test_hashtable.c ../src/hashtable.c -o test_hashtable $(C_FLAGS)
	./test_hashtable C89
	rm -f test_hashtable
	$(C_COMPILER) test_hashtable.c ../src/hashtable.c -o test_hashtable $(C_GNU_FLAGS)
	./test_hashtable GNU89
	rm -f test_hashtable
	$(CPP_COMPILER) test_hashtable.c ../src/hashtable.c -o test_hashtable $(CPP_FLAGS)
	./test_hashtable C++11
	rm -f test_hashtable
	$(CPP_COMPILER) test_hashtable.c ../src/hashtable.c -o test_hashtable $(CPP_GNU_FLAGS)
	./test_hashtable GNU++11
	rm -f test_hashtable

test_hash_string:
	$(C_COMPILER) test_hash_string.c ../src/hash_string.c ../src/hashtable.c -o test_hash_string $(C_FLAGS)
	./test_hash_string C89
	rm -f test_hash_string
	$(C_COMPILER) test_hash_string.c ../src/hash_string.c ../src/hashtable.c -o test_hash_string $(C_GNU_FLAGS)
	./test_hash_string GNU89
	rm -f test_hash_string
	$(CPP_COMPILER) test_hash_string.c ../src/hash_string.c ../src/hashtable.c -o test_hash_string $(CPP_FLAGS)
	./test_hash_string C++11
	rm -f test_hash_string
	$(CPP_COMPILER) test_hash_string.c ../src/hash_string.c ../src/hashtable.c -o test_hash_string $(CPP_GNU_FLAGS)
	./test_hash_string GNU++11
	rm -f test_hash_string

test_stack:
	$(C_COMPILER) test_stack.c ../src/stack.c -o test_stack $(C_FLAGS)
	./test_stack C89
	rm -f test_stack
	$(C_COMPILER) test_stack.c ../src/stack.c -o test_stack $(C_GNU_FLAGS)
	./test_stack GNU89
	rm -f test_stack
	$(CPP_COMPILER) test_stack.c ../src/stack.c -o test_stack $(CPP_FLAGS)
	./test_stack C++11
	rm -f test_stack
	$(CPP_COMPILER) test_stack.c ../src/stack.c -o test_stack $(CPP_GNU_FLAGS)
	./test_stack GNU++11
	rm -f test_stack

test_queue:
	$(C_COMPILER) test_queue.c ../src/queue.c -o test_queue $(C_FLAGS)
	./test_queue C89
	rm -f test_queue
	$(C_COMPILER) test_queue.c ../src/queue.c -o test_queue $(C_GNU_FLAGS)
	./test_queue GNU89
	rm -f test_queue
	$(CPP_COMPILER) test_queue.c ../src/queue.c -o test_queue $(CPP_FLAGS)
	./test_queue C++11
	rm -f test_queue
	$(CPP_COMPILER) test_queue.c ../src/queue.c -o test_queue $(CPP_GNU_FLAGS)
	./test_queue GNU++11
	rm -f test_queue
