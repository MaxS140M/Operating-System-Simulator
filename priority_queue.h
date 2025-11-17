#ifndef _NON_BLOCKING_QUEUE_SET_H_
#define _NON_BLOCKING_QUEUE_SET_H_

#include "list.h"
#include "non_blocking_queue.h"
#include <pthread.h>

typedef struct PriorityQueue {
  NonBlockingQueueT* queues;     // Array of non-blocking queues, one per priority level
  unsigned int num_queues;       // Num priority levels
  unsigned int current_queue;    // Current queue for highest 
  pthread_mutex_t mutex;         // Mutex for thread safety
  int terminated;                // Flag if queue is terminated
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
