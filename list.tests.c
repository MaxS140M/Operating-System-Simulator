#include "list.h"

#include <stdio.h>
#include <assert.h>

void test_empty_creation_destruction() {
  printf("Test empty creation/destruction\n");
  ListT* list = list_create();
  assert(list_empty(list));
  assert(list_length(list) == 0);
  list_destroy(list);
}

void test_prepend() {
  printf("Test prepend\n");
  ListT* list = list_create();
  list_prepend(list, 101);
  assert(!list_empty(list));
  assert(list_length(list) == 1);
  list_prepend(list, 202);
  assert(list_length(list) == 2);
  list_destroy(list);
}

void test_append() {
  printf("Test append\n");
  ListT* list = list_create();
  list_append(list, 101);
  assert(!list_empty(list));
  assert(list_length(list) == 1);
  list_append(list, 202);
  assert(list_length(list) == 2);
  list_destroy(list);
}

void test_find() {
  printf("Test find\n");
  ListT* list = list_create();
  list_prepend(list, 101);
  list_prepend(list, 202);
  list_prepend(list, 101);

  ListT* first = list_find_first(list, 101);
  assert(first);
  assert(first->value == 101);
  ListT* last = list_find_last(list, 101);
  assert(last);
  assert(last->value == 101);
  assert(first != last);
  ListT* first_fail = list_find_first(list, 303);
  assert(!first_fail);
  ListT* last_fail = list_find_last(list, 303);
  assert(!last_fail);
  
  list_destroy(list);  
}

void test_remove() {
  printf("Test remove\n");
  ListT* list = list_create();
  list_prepend(list, 101);
  list_prepend(list, 202);
  list_prepend(list, 101);

  ListT* first = list_find_first(list, 101);
  assert(first);
  assert(first->value == 101);
  ListT* last = list_find_last(list, 101);
  assert(last);
  assert(last->value == 101);
  assert(first != last);
  list_remove(list, first);
  assert(last == list_find_first(list, 101));
  list_remove(list, last);
  assert(!list_find_first(list, 101));
  assert(!list_find_last(list, 101));
  
  list_destroy(list);  
}

void test_pop() {
  ListT* list = list_create();
  list_append(list, 101);
  list_append(list, 202);
  list_append(list, 303);
  int const front = list_pop_front(list);
  assert(front == 101);
  assert(list_length(list) == 2);
  int const back = list_pop_back(list);
  assert(back == 303);
  assert(list_length(list) == 1);
  list_destroy(list);
}

void increment(unsigned int* value) {
  ++(*value);
}

void test_for_each() {
  ListT* list = list_create();
  list_append(list, 101);
  list_append(list, 202);
  list_append(list, 303);
  list_for_each(list, increment);
  assert(list_pop_front(list) == 102);
  assert(list_pop_front(list) == 203);
  assert(list_pop_front(list) == 304);
}

int main() {
  test_empty_creation_destruction();
  test_prepend();
  test_append();
  test_find();
  test_remove();
  test_pop();
  return 0;
}
