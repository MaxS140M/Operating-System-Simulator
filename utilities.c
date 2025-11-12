#include "utilities.h"

#include <assert.h>

void* checked_malloc(size_t size) {
  void* result = malloc(size);
  if(!result) abort();
  return result;
}

void checked_free(void* addr) {
  assert(addr);
  free(addr);
}
