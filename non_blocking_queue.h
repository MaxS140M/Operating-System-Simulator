#ifndef _NON_BLOCKING_QUEUE_H_
#define _NON_BLOCKING_QUEUE_H_

#include "list.h"

typedef struct NonBlockingQueue {
  /* Add fields as needed */
  int remove_me; // Added to silence empty structure compiler warning
} NonBlockingQueueT;

void non_blocking_queue_create(NonBlockingQueueT* queue);
void non_blocking_queue_destroy(NonBlockingQueueT* queue);

void non_blocking_queue_push(NonBlockingQueueT* queue, unsigned int value);
int non_blocking_queue_pop(NonBlockingQueueT* queue, unsigned int* value);

int non_blocking_queue_empty(NonBlockingQueueT* queue);
int non_blocking_queue_length(NonBlockingQueueT* queue);

#endif
