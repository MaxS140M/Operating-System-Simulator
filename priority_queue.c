#include "priority_queue.h"
#include "utilities.h"

#include <assert.h>
#include <pthread.h>

void priority_queue_create(PriorityQueueT* queue, unsigned int number_of_queues) {
    assert(queue != NULL);
    assert(number_of_queues > 0);
    
    queue->num_queues = number_of_queues;
    queue->current_queue = 0;
    queue->terminated = 0;
    
    // Allocate array of non-blocking queues
    queue->queues = checked_malloc(sizeof(NonBlockingQueueT) * number_of_queues);
    
    // Initialize each queue
    for (unsigned int i = 0; i < number_of_queues; i++) {
        non_blocking_queue_create(&queue->queues[i]);
    }
    
    pthread_mutex_init(&queue->mutex, NULL);

}

void priority_queue_destroy(PriorityQueueT* queue) {
    assert(queue != NULL);
    
    pthread_mutex_lock(&queue->mutex);
    
    if (queue->queues) {
        // Destroy each individual queue
        for (unsigned int i = 0; i < queue->num_queues; i++) {
            non_blocking_queue_destroy(&queue->queues[i]);
        }
        checked_free(queue->queues);
        queue->queues = NULL;
    }
    
    pthread_mutex_unlock(&queue->mutex);
    pthread_mutex_destroy(&queue->mutex);
}

void priority_queue_push(PriorityQueueT* queue, PriorityT level, unsigned int value) {
    assert(queue != NULL);
    assert(queue->queues != NULL);
    assert(level < queue->num_queues);
    
    pthread_mutex_lock(&queue->mutex);
    
    if (!queue->terminated) {
        non_blocking_queue_push(&queue->queues[level], value);
    }
    
    pthread_mutex_unlock(&queue->mutex);
}

int priority_queue_pop(PriorityQueueT* queue, unsigned int *value) {
    assert(queue != NULL);
    assert(queue->queues != NULL);
    assert(value != NULL);
    
    pthread_mutex_lock(&queue->mutex);
    
    if (queue->terminated) {
        pthread_mutex_unlock(&queue->mutex);
        return 1; // Failure due to termination
    }
    
    // Try to pop from highest priority (lowest index) first
    for (unsigned int priority = 0; priority < queue->num_queues; priority++) {
        if (!non_blocking_queue_empty(&queue->queues[priority])) {
            int result = non_blocking_queue_pop(&queue->queues[priority], value);
            pthread_mutex_unlock(&queue->mutex);
            return result;
        }
    }
    
    // All queues are empty
    pthread_mutex_unlock(&queue->mutex);
    return 1; // Failure - no items available
}

int priority_queue_empty(PriorityQueueT* queue) {
    assert(queue != NULL);
    assert(queue->queues != NULL);
    
    pthread_mutex_lock(&queue->mutex);
    
    for (unsigned int i = 0; i < queue->num_queues; i++) {
        if (!non_blocking_queue_empty(&queue->queues[i])) {
            pthread_mutex_unlock(&queue->mutex);
            return 0; // Not empty
        }
    }
    
    pthread_mutex_unlock(&queue->mutex);
    return 1; // Empty

}

int priority_queue_length(PriorityQueueT* queue) {
    assert(queue != NULL);
    assert(queue->queues != NULL);
    
    pthread_mutex_lock(&queue->mutex);
    
    int total_length = 0;
    for (unsigned int i = 0; i < queue->num_queues; i++) {
        total_length += non_blocking_queue_length(&queue->queues[i]);
    }
    
    pthread_mutex_unlock(&queue->mutex);
    return total_length;
}

void priority_queue_terminate(PriorityQueueT* queue) {
    assert(queue != NULL);
    
    pthread_mutex_lock(&queue->mutex);
    queue->terminated = 1;
    pthread_mutex_unlock(&queue->mutex);
}

