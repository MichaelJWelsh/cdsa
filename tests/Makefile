COMPILER=gcc
FLAGS=-Wall -Werror -pedantic-errors -std=c11
TEST_INCLUDES=alloc.c

clean:
	rm -f test_list

test: test_list

test_list:
	$(COMPILER) $(TEST_INCLUDES) test_list.c ../src/list.c -o test_list $(FLAGS)
	./test_list
