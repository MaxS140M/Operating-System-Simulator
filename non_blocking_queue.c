// student: Max Sloam ID: 20666789

#include "non_blocking_queue.h"
#include "utilities.h"

#include <assert.h>
#include <pthread.h>

void non_blocking_queue_create(NonBlockingQueueT* queue) {
    assert(queue != NULL);
    queue->list = list_create();
    assert(queue->list != NULL);
    pthread_mutex_init(&queue->mutex, NULL);
}

void non_blocking_queue_destroy(NonBlockingQueueT* queue) {
    assert(queue != NULL);
    assert(queue->list != NULL);
    pthread_mutex_lock(&queue->mutex);
    list_destroy(queue->list);
    queue->list = NULL;
    pthread_mutex_unlock(&queue->mutex);
    pthread_mutex_destroy(&queue->mutex);
}

void non_blocking_queue_push(NonBlockingQueueT* queue, unsigned int value) {
    assert(queue != NULL);
    assert(queue->list != NULL);
    pthread_mutex_lock(&queue->mutex);
    list_append(queue->list, value);
    pthread_mutex_unlock(&queue->mutex);
}

int non_blocking_queue_pop(NonBlockingQueueT* queue, unsigned int* value) {
    assert(queue != NULL);
    assert(queue->list != NULL);
    assert(value != NULL);
    
    pthread_mutex_lock(&queue->mutex);
    
    if (list_empty(queue->list)) {
        pthread_mutex_unlock(&queue->mutex);
        return 1; // Non-zero indicates failure (queue empty)
    }
    
    *value = list_pop_front(queue->list);
    pthread_mutex_unlock(&queue->mutex);
    return 0; // Zero indicates success
}

int non_blocking_queue_empty(NonBlockingQueueT* queue) {
    assert(queue != NULL);
    assert(queue->list != NULL);
    pthread_mutex_lock(&queue->mutex);
    int empty = list_empty(queue->list);
    pthread_mutex_unlock(&queue->mutex);
    return empty;
}

int non_blocking_queue_length(NonBlockingQueueT* queue) {
    assert(queue != NULL);
    assert(queue->list != NULL);
    pthread_mutex_lock(&queue->mutex);
    size_t length = list_length(queue->list);
    pthread_mutex_unlock(&queue->mutex);
    return (int)length;
}
