#include "blocking_queue.h"
#include "utilities.h"

#include <assert.h>


#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void test_create_destroy() {
  printf(" \n Testing create/destroy: ");
  BlockingQueueT q;
  blocking_queue_create(&q);
  assert(blocking_queue_empty(&q));
  assert(blocking_queue_length(&q) == 0);
  blocking_queue_destroy(&q);
  printf("passed\n");
}

void test_push_pop() {
  printf("Testing push/pop: ");
  BlockingQueueT q;
  blocking_queue_create(&q);
  blocking_queue_push(&q, 7);
  blocking_queue_push(&q, 8);
  assert(!blocking_queue_empty(&q));
  assert(blocking_queue_length(&q) == 2);
  unsigned int v;
  int r = blocking_queue_pop(&q, &v);
  assert(r == 0 && v == 7);
  r = blocking_queue_pop(&q, &v);
  assert(r == 0 && v == 8);
  assert(blocking_queue_empty(&q));
  blocking_queue_destroy(&q);
  printf("passed\n");
}

struct pop_thread_arg {
  BlockingQueueT* q;
  unsigned int value;
  int result;
};

void* pop_thread_func(void* arg) {
  struct pop_thread_arg* a = (struct pop_thread_arg*)arg;
  unsigned int v;
  int r = blocking_queue_pop(a->q, &v);
  a->value = v;
  a->result = r;
  return NULL;
}

void test_blocking_pop_unblocked_by_push() {
  printf("Testing blocking pop unblocked by push: ");
  BlockingQueueT q;
  blocking_queue_create(&q);

  pthread_t t;
  struct pop_thread_arg arg;
  arg.q = &q;
  arg.value = 0;
  arg.result = -1;

  pthread_create(&t, NULL, pop_thread_func, &arg);

  /* Give thread time to block */
  sleep(1);
  blocking_queue_push(&q, 1234);
  pthread_join(t, NULL);
  assert(arg.result == 0 && arg.value == 1234);

  blocking_queue_destroy(&q);
  printf("passed\n");
}

void test_terminate_unblocks_waiting_pop() {
  printf("Testing terminate unblocks waiting pop: ");
  BlockingQueueT q;
  blocking_queue_create(&q);

  pthread_t t;
  struct pop_thread_arg arg;
  arg.q = &q;
  arg.value = 0;
  arg.result = -1;

  pthread_create(&t, NULL, pop_thread_func, &arg);
  /* Give thread time to block */
  sleep(1);
  blocking_queue_terminate(&q);
  pthread_join(t, NULL);
  /* After termination the pop should fail and return non-zero */
  assert(arg.result != 0);

  blocking_queue_destroy(&q);
  printf("passed\n");
}

int main() {
  printf("Running blocking queue tests: \n ");
  test_create_destroy();
  test_push_pop();
  test_blocking_pop_unblocked_by_push();
  test_terminate_unblocks_waiting_pop();
  printf("\n passed all.\n");
  return 0;
}
