#include "blocking_queue.h"
#include "utilities.h"

#include <assert.h>

void test_success_example() {
  // Example of a simple successful test
  assert(0 == 0);
}

void test_failure_example() {
  // Example of a simple failing test
  assert(1 == 0);
}

int main() {
  test_failure_example();
  test_success_example();
  return 0;
}
