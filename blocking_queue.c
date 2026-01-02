#include "blocking_queue.h"
#include "utilities.h"

#include <assert.h>
#include <pthread.h>
#include <semaphore.h>

void blocking_queue_terminate(BlockingQueueT* queue) {
  assert(queue != NULL);
  pthread_mutex_lock(&queue->mutex);
  queue->terminated = 1;
  // Wake up all waiting threads by posting to semaphore multiple times
  for (int i = 0; i < 1000; i++) {
    sem_post(&queue->semaphore);
  }
  pthread_mutex_unlock(&queue->mutex);
}

void blocking_queue_create(BlockingQueueT* queue) {
  assert(queue != NULL);
  queue->list = list_create();
  assert(queue->list != NULL);
  pthread_mutex_init(&queue->mutex, NULL);
  sem_init(&queue->semaphore, 0, 0);  // Initialize with 0 count
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

  sem_destroy(&queue->semaphore);
  pthread_mutex_destroy(&queue->mutex);
}

void blocking_queue_push(BlockingQueueT* queue, unsigned int value) {
  assert(queue != NULL);
  assert(queue->list != NULL);
  pthread_mutex_lock(&queue->mutex);
  if (!queue->terminated) {
    list_append(queue->list, value);
    sem_post(&queue->semaphore);  // Signal that an item is available
  }
  pthread_mutex_unlock(&queue->mutex);
}

int blocking_queue_pop(BlockingQueueT* queue, unsigned int* value) {
  assert(queue != NULL);
  assert(value != NULL);
  assert(queue->list != NULL);

  // Wait for an item to be available or termination
  sem_wait(&queue->semaphore);
  
  pthread_mutex_lock(&queue->mutex);
  
  // Check if we were woken up due to termination
  if (queue->terminated) {
    pthread_mutex_unlock(&queue->mutex);
    return 1; /* failure due to termination */
  }
  
  // Check if list is empty (should not happen after sem_wait, but safety check)
  if (list_empty(queue->list)) {
    pthread_mutex_unlock(&queue->mutex);
    return 1; /* failure - no items available */
  }

  *value = list_pop_front(queue->list);
  pthread_mutex_unlock(&queue->mutex);
  return 0; /* success */
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
