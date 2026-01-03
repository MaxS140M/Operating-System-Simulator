#ifndef _BLOCKING_QUEUE_H_
#define _BLOCKING_QUEUE_H_

#include "list.h"
#include <pthread.h>
#include <semaphore.h>

typedef struct BlockingQueue {
  ListT* list;              // underlying list used as the queue
  pthread_mutex_t mutex;    // mutex protecting the queue
  sem_t semaphore;          // semaphore for blocking/waking threads
  int terminated;           // non-zero if the queue has been terminated
} BlockingQueueT;

void blocking_queue_create(BlockingQueueT* queue);
void blocking_queue_destroy(BlockingQueueT* queue);

void blocking_queue_push(BlockingQueueT* queue, unsigned int value);
int blocking_queue_pop(BlockingQueueT* queue, unsigned int* value);

int blocking_queue_empty(BlockingQueueT* queue);
int blocking_queue_length(BlockingQueueT* queue);

void blocking_queue_terminate(BlockingQueueT* queue);

#endif
