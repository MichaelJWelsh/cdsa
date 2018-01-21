# C Data Structures and Algorithms
> A library of generic intrusive data structures and algorithms in ANSI C

- [Background](#background)
- [Examples](#examples)
- [Installation](#installation)
- [Running Tests](#running-tests)
- [Contributing](#contributing)

## Background
This repository is an ongoing project of implementing generic *intrusive* data structures and algorithms in ANSI C. I find I often use the same constructs that require a lot of boilerplate code, so I made this repository to both organize these constructs and allow easy integration of these constructs into existing projects.

My design philosophy with this repository is to offer a minimalistic API for each data structure, containing all tools needed to easily build more complex/niche constructs. Portability is top priority, so only ANSI C is used, and no header/source pair relies on another header/source pair. Furthermore, to promote being used in embedded systems, iterative algorithms are used exclusively over recursive algorithms, and each data structure is intrusive (for better memory locality).

Feel free to use these data structures and algorithms in your own way. The code is licensed under the ISC license (a simplified version of the BSD license that is functionally identical); thus, it may legitimately be reused in any project, whether Proprietary or Open Source.

## Examples
Each header is heavily documented and should hopefully answer all questions regarding functionality. An in-depth explanation on how each data structure operates is located at the top of the header file.

#### List
```c
// Define your struct somewhere.
struct Object {
    int some_value;
    ...

    // Don't forget to embed the ListNode!
    ListNode node;
};

...

// Create some Object variables.
struct Object obj1, obj2;
obj1.some_value = 1;
obj2.some_value = 2;

// Create your List.
List my_list;
list_init(&my_list);

// Populate your List. Notice how the API abstracts itself and only cares about the ListNode.
list_insert_back(&my_list, &obj1.node);
list_insert_back(&my_list, &obj2.node);

// Let's see what is stored in the List:
int i = 1;
ListNode *n;
list_for_each(n, &my_list) {
    if (i == 1) {
        assert(n == &obj1.node);
    } else if (i == 2){
        assert(n == &obj2.node);
    }

    ++i;
}

...

// Let's get the ListNode at the front of the List.
ListNode *front_node_ptr = list_front(&my_list);

// Getting the "node" member from an Object variable is easy: ("obj1.node"),
// but how do you get the Object variable when you only have the "node" member?
// Solution: the macro "list_entry":
struct Object *obj_ptr = list_entry(front_node_ptr, struct Object, node);
assert(obj_ptr == &obj1);
```
#### RBTree
```c
// Define your struct somewhere.
struct Object {
    int key;
    ...

    // Don't forget to embed the RBTreeNode!
    RBTreeNode node;
};

...

// You must define a compare function which compares a key with the key of a RBTreeNode.
int compare(const void *some_key, const RBTreeNode *some_node) {
    return *(const int*)some_key - rbtree_entry(some_node, struct Object, node)->key;
}

// You can OPTIONALLY define a collide function which handles key collisions. When a key
// collision happens, no matter what, the old RBTreeNode will be replaced by the new
// RBTreeNode. If this function is defined, then it will be called after the old
// RBTreeNode is replaced in the RBTree. This function is great if you need to free up
// resources in the Object variable pertaining to the discarded RBTreeNode. This function
// is also great for enabling multi-key functionality in the RBTree.
void collide(const RBTreeNode *old_node, const RBTreeNode *new_node, void *auxiliary_data) {
    // When a RBTree is created, we can give it auxiliary data to hold onto. This data is then
    // passed to this function for usage. This data, for example, could be a memory pool struct
    // that is needed to free up the resources used by the Object variable pertaining to the
    // old_node parameter.

    ...
}

...

// Create some Object variables.
struct Object obj1, obj2;
obj1.key = 1;
obj2.key = 2;

// Create you RBTree. In this case we don't have any auxiliary data that needs to be used
// in the collide function, so we just pass NULL. As previously mentioned, the collide
// function itself is optional. If you don't need to do any special resource management,
// just pass "NULL" in place of "collide" in the initialize function.
RBTree my_rbtree;
rbtree_init(&my_rbtree, compare, collide, NULL);

// Populate your RBTree. Notice how the API abstracts itself and only cares about the
// RBTreeNode and its key. Because red black trees are always ordered in some way, order
// of insertion will never affect the inorderness of the tree.
rbtree_insert(&my_rbtree, &obj2.key, &obj2.node);
rbtree_insert(&my_rbtree, &obj1.key, &obj1.node);

// Because of the way our compare function is designed, the RBTree stores its RBTreeNodes
// inorder from smallest key to greatest key. Let's see what is stored in the RBTree:
int i = 1;
RBTreeNode *n;
rbtree_for_each(n, &my_rbtree) {
    if (i == 1) {
        assert(n == &obj1.node);
    } else if (i == 2) {
        assert(n == &obj2.node);
    }

    ++i;
}

...

// Let's get the RBTreeNode with the greatest key.
RBTreeNode *greatest_node_ptr = rbtree_last(&my_rbtree);

// Getting the "node" member from an Object variable is easy: ("obj1.node"),
// but how do you get the Object variable when you only have the "node" member?
// Solution: the macro "rbtree_entry":
struct Object *obj_ptr = rbtree_entry(greatest_node_ptr, struct Object, node);
assert(obj_ptr == &obj2);
```
#### HashTable
```c
// Define your struct somewhere.
struct Object {
    int key;
    ...

    // Don't forget to embed the HashTableNode!
    HashTableNode node;
};

...

// You must define a hash function which takes in a key and returns its hashcode. In this
// case we aren't going to do anything fancy since this is just an example.
size_t hash(const void *key) {
    return *(const int*)key;
}

// You must define an equal function which determines if a key is equal to the key of a HashTableNode.
int equal(const void *some_key, const HashTableNode *some_node) {
    return *(const int*)some_key == hashtable_entry(some_node, struct Object, node)->key;
}

// You can OPTIONALLY define a collide function which handles key collisions. When a key
// collision happens, no matter what, the old HashTableNode will be replaced by the new
// HashTableNode. If this function is defined, then it will be called after the old
// HashTableNode is replaced in the HashTable. This function is great if you need to free up
// resources in the Object variable pertaining to the discarded HashTableNode. This function
// is also great for enabling multi-key functionality in the HashTable.
void collide(const HashTableNode *old_node, const HashTableNode *new_node, void *auxiliary_data) {
    // When a HashTable is created, we can give it auxiliary data to hold onto. This data is then
    // passed to this function for usage. This data, for example, could be a memory pool struct
    // that is needed to free up the resources used by the Object variable pertaining to the
    // old_node parameter.

    ...
}

...

// Create some Object variables.
struct Object obj1, obj2;
obj1.key = 1;
obj2.key = 2;

// You must create a bucket array which is an array of pointers to HashTableNodes. This
// array is used by the HashTable for the duration of its lifetime. In this case we will
// have 50 buckets in the bucket array.
HashTableNode *bucket_array[50];

// Create you HashTable. In this case we don't have any auxiliary data that needs to be used
// in the collide function, so we just pass NULL. As previously mentioned, the collide
// function itself is optional. If you don't need to do any special resource management,
// just pass "NULL" in place of "collide" in the initialize function.
HashTable my_hashtable;
hashtable_init(&my_hashtable, bucket_array, 50, hash, equal, collide, NULL);

// Populate your HashTable. Notice how the API abstracts itself and only cares about the
// HashTableNode and its key.
hashtable_insert(&my_hashtable, &obj1.key, &obj1.node);
hashtable_insert(&my_hashtable, &obj2.key, &obj2.node);

// HashTable provides no guarantee on the ordering of HashTableNodes in the bucket array.
// Let's see what is stored in the HashTable:
int sum_of_keys = 0, bucket_index;
HashTableNode *n;
hashtable_for_each(n, bucket_index, &my_hashtable) {
    sum_of_keys += hashtable_entry(n, struct Object, node)->key;
}
assert(sum_of_keys == 3);

...

// Let's get the HashTableNode with the key that equals 1.
int key = 1;
HashTableNode *node_ptr = hashtable_lookup_key(&my_hashtable, &key);

// Getting the "node" member from an Object variable is easy: ("obj1.node"),
// but how do you get the Object variable when you only have the "node" member?
// Solution: the macro "hashtable_entry":
struct Object *obj_ptr = hashtable_entry(node_ptr, struct Object, node);
assert(obj_ptr == &obj1);
```
#### Stack
```c
// Define your struct somewhere.
struct Object {
    int some_value;
    ...

    // Don't forget to embed the StackNode!
    StackNode node;
};

...

// Create some Object variables.
struct Object obj1, obj2;
obj1.some_value = 1;
obj2.some_value = 2;

// Create your Stack.
Stack my_stack;
stack_init(&my_stack);

// Populate your Stack. Notice how the API abstracts itself and only cares about the StackNode.
stack_push(&my_stack, &obj1.node);
stack_push(&my_stack, &obj2.node);

// Let's see what is stored in the Stack:
int i = 1;
StackNode *n;
stack_for_each(n, &my_stack) {
    if (i == 1) {
        assert(n == &obj2.node);
    } else if (i == 2){
        assert(n == &obj1.node);
    }

    ++i;
}

...

// Let's get the StackNode at the top of the Stack.
StackNode *top_node_ptr = stack_peek(&my_stack);

// Getting the "node" member from an Object variable is easy: ("obj1.node"),
// but how do you get the Object variable when you only have the "node" member?
// Solution: the macro "stack_entry":
struct Object *obj_ptr = stack_entry(top_node_ptr, struct Object, node);
assert(obj_ptr == &obj2);
```
#### Queue
```c
// Define your struct somewhere.
struct Object {
    int some_value;
    ...

    // Don't forget to embed the QueueNode!
    QueueNode node;
};

...

// Create some Object variables.
struct Object obj1, obj2;
obj1.some_value = 1;
obj2.some_value = 2;

// Create your Queue.
Queue my_queue;
queue_init(&my_queue);

// Populate your Queue. Notice how the API abstracts itself and only cares about the QueueNode.
queue_push(&my_queue, &obj1.node);
queue_push(&my_queue, &obj2.node);

// Let's see what is stored in the Queue:
int i = 1;
QueueNode *n;
queue_for_each(n, &my_queue) {
    if (i == 1) {
        assert(n == &obj1.node);
    } else if (i == 2){
        assert(n == &obj2.node);
    }

    ++i;
}

...

// Let's get the QueueNode at the front of the Queue.
QueueNode *front_node_ptr = queue_peek(&my_queue);

// Getting the "node" member from an Object variable is easy: ("obj1.node"),
// but how do you get the Object variable when you only have the "node" member?
// Solution: the macro "queue_entry":
struct Object *obj_ptr = queue_entry(front_node_ptr, struct Object, node);
assert(obj_ptr == &obj1);
```

## Installation
This library is written in ANSI C, so the code should work with just about every compiler. Each header/source pair is independent of the others. This makes using an individual data structure easy. Just simply drag and drop the header/source pair into your project directly, and make sure to compile the source file along with your other files.

## Running Tests
You must have the GNU compiler available to run the tests. Make sure you have all the files downloaded pertaining to this library as well.

Simply run this command once in the "tests" directory to run all the tests:
```
make
```

Example:
```
cd tests/
make
gcc test_list.c ../src/list.c -o test_list -Wall -Wextra -Werror -pedantic-errors -std=c89
./test_list C89

PASSED: List C89

rm -f test_list
gcc test_list.c ../src/list.c -o test_list -Wall -Wextra -Werror -std=gnu89
./test_list GNU89

PASSED: List GNU89

rm -f test_list
g++ test_list.c ../src/list.c -o test_list -Wall -Wextra -Werror -pedantic-errors -std=c++11
./test_list C++11

PASSED: List C++11

rm -f test_list
g++ test_list.c ../src/list.c -o test_list -Wall -Wextra -Werror -std=gnu++11
./test_list GNU++11

PASSED: List GNU++11

rm -f test_list
gcc test_rbtree.c ../src/rbtree.c -o test_rbtree -Wall -Wextra -Werror -pedantic-errors -std=c89
./test_rbtree C89

PASSED: RBTree C89

rm -f test_rbtree
gcc test_rbtree.c ../src/rbtree.c -o test_rbtree -Wall -Wextra -Werror -std=gnu89
./test_rbtree GNU89

PASSED: RBTree GNU89

etc... (this goes on for a while)
```

## Contributing
Contributions are welcome!

If you have a feature request, or have found a bug, feel free to open a [new issue](https://github.com/MichaelJWelsh/cdsa/issues/new). If you wish to contribute code, please document thoroughly and make sure to write a test for each function/macro. I will help finalize the code and will help make sure that the conventions used are consistent.
