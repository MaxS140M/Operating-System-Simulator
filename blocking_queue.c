#include "blocking_queue.h"
#include "utilities.h"

#include <assert.h>
#include <pthread.h>


void blocking_queue_terminate(BlockingQueueT* queue) {
  assert(queue != NULL);
  pthread_mutex_lock(&queue->mutex);
  queue->terminated = 1;
  pthread_cond_broadcast(&queue->cond);
  pthread_mutex_unlock(&queue->mutex);

}

void blocking_queue_create(BlockingQueueT* queue) {
  assert(queue != NULL);
  queue->list = list_create();
  assert(queue->list != NULL);
  pthread_mutex_init(&queue->mutex, NULL);
  pthread_cond_init(&queue->cond, NULL);
  queue->terminated = 0;

}

void blocking_queue_destroy(BlockingQueueT* queue) {
  assert(queue != NULL);
  /* Ensure any waiting threads are woken up */
  blocking_queue_terminate(queue);

  pthread_mutex_lock(&queue->mutex);
  if (queue->list) {
    list_destroy(queue->list);
    queue->list = NULL;
  }
  pthread_mutex_unlock(&queue->mutex);

  pthread_cond_destroy(&queue->cond);
  pthread_mutex_destroy(&queue->mutex);

}

void blocking_queue_push(BlockingQueueT* queue, unsigned int value) {
  assert(queue != NULL);
  assert(queue->list != NULL);
  pthread_mutex_lock(&queue->mutex);
  if (!queue->terminated) {
    list_append(queue->list, value);
    pthread_cond_signal(&queue->cond);
  }
  pthread_mutex_unlock(&queue->mutex);

}

int blocking_queue_pop(BlockingQueueT* queue, unsigned int* value) {
  assert(queue != NULL);
  assert(value != NULL);
  assert(queue->list != NULL);

  pthread_mutex_lock(&queue->mutex);
  while (list_empty(queue->list) && !queue->terminated) {
    pthread_cond_wait(&queue->cond, &queue->mutex);
  }

  if (queue->terminated) {
    pthread_mutex_unlock(&queue->mutex);
    return 1; /* failure due to termination */
  }

  *value = list_pop_front(queue->list);
  pthread_mutex_unlock(&queue->mutex);

  return 0;
}

int blocking_queue_empty(BlockingQueueT* queue) {
  assert(queue != NULL);
  assert(queue->list != NULL);
  pthread_mutex_lock(&queue->mutex);
  int empty = list_empty(queue->list);
  pthread_mutex_unlock(&queue->mutex);

  return empty;
}

int blocking_queue_length(BlockingQueueT* queue) {
   assert(queue != NULL);
  assert(queue->list != NULL);
  pthread_mutex_lock(&queue->mutex);
  size_t len = list_length(queue->list);
  pthread_mutex_unlock(&queue->mutex);
  return (int)len;
}
