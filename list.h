#ifndef _LIST_H_
#define _LIST_H_

#include <stddef.h>

typedef struct List {
  struct List* pred;
  struct List* succ;
  unsigned int value;
} ListT;

// Construct an empty list
ListT* list_create();
// Destroy a list and free all its memory
void list_destroy(ListT* list);

// Prepend value to the front of the list
void list_prepend(ListT* list, unsigned int value);

// Append value to the end of the list
void list_append(ListT* list, unsigned int value);

// Remove the node from the list, and free its memory
void list_remove(ListT* list, struct List* node);

// Test if the list is empty in constant time
int list_empty(ListT* list);

// Calculate the list length in linear time
size_t list_length(ListT* list);

// Find the first occurrence
struct List* list_find_first(ListT* list, unsigned int value);

// Find the last occurrence
struct List* list_find_last(ListT* list, unsigned int value);

// Remove the first element - undefined if absent
unsigned int list_pop_front(ListT* list);

// Remove the last element - undefined if absent
unsigned int list_pop_back(ListT* list);

// Run action on each element of the list
void list_for_each(ListT* list, void (*action)(unsigned int*));

#endif
