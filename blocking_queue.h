#ifndef _BLOCKING_QUEUE_H_
#define _BLOCKING_QUEUE_H_

#include "list.h"
#include <pthread.h>

typedef struct BlockingQueue {
  ListT* list;              //  queue
  pthread_mutex_t mutex;    // mutex protecting the queue
  pthread_cond_t cond;      // condition variable for pop / push
  int terminated;	    //non -zero if queue is terminated
} BlockingQueueT;

void blocking_queue_create(BlockingQueueT* queue);
void blocking_queue_destroy(BlockingQueueT* queue);

void blocking_queue_push(BlockingQueueT* queue, unsigned int value);
int blocking_queue_pop(BlockingQueueT* queue, unsigned int* value);

int blocking_queue_empty(BlockingQueueT* queue);
int blocking_queue_length(BlockingQueueT* queue);

void blocking_queue_terminate(BlockingQueueT* queue);

#endif
