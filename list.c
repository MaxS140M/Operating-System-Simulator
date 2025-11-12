#include "list.h"

#include <stdlib.h>
#include <assert.h>


struct List* alloc_node() {
  return malloc(sizeof(struct List));
}

void free_node(struct List* node) {
  assert(node);
  free(node);
}

void list_prepend(ListT* list, unsigned int value) {
  assert(list);
  struct List* node = alloc_node();
  assert(node);
  node->value = value;
  node->succ = list->succ;
  list->succ->pred = node;
  list->succ = node;
  node->pred = list;
}

void list_append(ListT* list, unsigned int value) {
  assert(list);
  struct List* node = alloc_node();
  assert(node);
  node->value = value;
  node->pred = list->pred;
  list->pred->succ = node;
  list->pred = node;
  node->succ = list;
}

void list_remove(ListT* list, struct List* node) {
  assert(list);
  assert(node);
  assert(node != list);
  node->pred->succ = node->succ;
  node->succ->pred = node->pred;
  free_node(node);
}

ListT* list_create() {
  ListT* sentinel = alloc_node();
  assert(sentinel);
  sentinel->value = 0;
  sentinel->succ = sentinel;
  sentinel->pred = sentinel;
  return sentinel;
}

int list_empty(ListT* list) {
  return list->succ == list;
}

void list_destroy(ListT* list) {
  assert(list);
  while(!list_empty(list)) {
    list_remove(list, list->succ);
  }
  free_node(list);
}

size_t list_length(ListT* list) {
  assert(list);
  struct List* succ = list->succ;
  assert(succ);
  size_t len = 0;
  while(succ != list) {
    ++len;
    succ = succ->succ;
    assert(succ);
  }
  return len;
}

struct List* list_find_first(ListT* list, unsigned int value) {
  assert(list);
  struct List* succ = list->succ;
  while(succ != list) {
    if(succ->value == value)
      return succ;
    else {
      succ = succ->succ;
      assert(succ);
    }
  }
  return 0;  
}

struct List* list_find_last(ListT* list, unsigned int value) {
  assert(list);
  struct List* pred = list->pred;
  while(pred != list) {
    if(pred->value == value)
      return pred;
    else {
      pred = pred->pred;
      assert(pred);
    }
  }
  return 0;
}

unsigned int list_pop_front(ListT* list) {
  assert(list);
  assert(!list_empty(list));
  int const value = list->succ->value;
  list_remove(list, list->succ);
  return value;
}

unsigned int list_pop_back(ListT* list) {
  assert(list);
  assert(!list_empty(list));
  int const value = list->pred->value;
  list_remove(list, list->pred);
  return value;
}

void list_for_each(ListT* list, void (*action)(unsigned int*)) {
  assert(list);
  for(struct List* node = list->succ;
      node != list;
      node = node->succ) {
    assert(node);
    action(&node->value);
  }
}
