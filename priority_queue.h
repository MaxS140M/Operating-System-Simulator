#ifndef _NON_BLOCKING_QUEUE_SET_H_
#define _NON_BLOCKING_QUEUE_SET_H_

#include "list.h"

typedef struct PriorityQueue {
  /* Add fields as needed */
  int remove_me; // Added to silence empty structure compiler warning
} PriorityQueueT;

typedef unsigned int PriorityT;

/* Standard interface, do not change*/
void priority_queue_create(PriorityQueueT* priority_queue, unsigned int number_of_queues);
void priority_queue_destroy(PriorityQueueT* queue);

void priority_queue_push(PriorityQueueT* priority_queue, PriorityT level, unsigned int value);
int priority_queue_pop(PriorityQueueT* queue, unsigned int* value);

int priority_queue_empty(PriorityQueueT* queue);
int priority_queue_length(PriorityQueueT* queue);

void priority_queue_terminate(PriorityQueueT* priority_queue);
/* End of standard interface */

/* You may add functions to the queue interface if required */

#endif
