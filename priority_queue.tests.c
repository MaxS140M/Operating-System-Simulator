#include "priority_queue.h"
#include "utilities.h"

#include <assert.h>

#include <stdio.h>

void test_create_destroy() {
    printf("\n Testing create/destroy: ");
    PriorityQueueT pq;
    priority_queue_create(&pq, 3);
    assert(priority_queue_empty(&pq));
    assert(priority_queue_length(&pq) == 0);
    priority_queue_destroy(&pq);
    printf("passed\n");
}

void test_basic_push_pop() {
    printf("Testing basic push/pop: ");
    PriorityQueueT pq;
    priority_queue_create(&pq, 3);
    
    // Push to different priority levels
    priority_queue_push(&pq, 0, 100); // Highest priority
    priority_queue_push(&pq, 1, 200); // Medium priority
    priority_queue_push(&pq, 2, 300); // Lowest priority
    
    assert(!priority_queue_empty(&pq));
    assert(priority_queue_length(&pq) == 3);
    
    // Should pop in priority order (0, 1, 2)
    unsigned int value;
    int result;
    
    result = priority_queue_pop(&pq, &value);
    assert(result == 0 && value == 100); // Priority 0 first
    
    result = priority_queue_pop(&pq, &value);
    assert(result == 0 && value == 200); // Priority 1 second
    
    result = priority_queue_pop(&pq, &value);
    assert(result == 0 && value == 300); // Priority 2 last
    
    assert(priority_queue_empty(&pq));
    
    priority_queue_destroy(&pq);
    printf(" passed\n");
}

void test_priority_ordering() {
    printf("Testing priority ordering: ");
    PriorityQueueT pq;
    priority_queue_create(&pq, 3);
    
    // Push in reverse priority order to test ordering
    priority_queue_push(&pq, 2, 300); // Lowest priority first
    priority_queue_push(&pq, 0, 100); // Highest priority
    priority_queue_push(&pq, 1, 200); // Medium priority
    priority_queue_push(&pq, 0, 101); // Another high priority
    
    unsigned int value;
    int result;
    
    // Should still get priority 0 items first
    result = priority_queue_pop(&pq, &value);
    assert(result == 0 && value == 100);
    
    result = priority_queue_pop(&pq, &value);
    assert(result == 0 && value == 101);
    
    // Then priority 1
    result = priority_queue_pop(&pq, &value);
    assert(result == 0 && value == 200);
    
    // Finally priority 2
    result = priority_queue_pop(&pq, &value);
    assert(result == 0 && value == 300);
    
    priority_queue_destroy(&pq);
    printf("passed\n");
}

void test_fifo_within_priority() {
    printf("Testing FIFO within same priority level: ");
    PriorityQueueT pq;
    priority_queue_create(&pq, 2);
    
    // Push multiple items to same priority level
    priority_queue_push(&pq, 1, 201);
    priority_queue_push(&pq, 1, 202);
    priority_queue_push(&pq, 1, 203);
    
    unsigned int value;
    int result;
    
    // Should pop in FIFO order within the same priority
    result = priority_queue_pop(&pq, &value);
    assert(result == 0 && value == 201);
    
    result = priority_queue_pop(&pq, &value);
    assert(result == 0 && value == 202);
    
    result = priority_queue_pop(&pq, &value);
    assert(result == 0 && value == 203);
    
    priority_queue_destroy(&pq);
    printf("passed\n");
}

void test_empty_pop() {
    printf("Testing pop from empty queue: ");
    PriorityQueueT pq;
    priority_queue_create(&pq, 2);
    
    unsigned int value;
    int result = priority_queue_pop(&pq, &value);
    assert(result != 0); // Should fail
    
    priority_queue_destroy(&pq);
    printf("passed\n");
}

void test_terminate() {
    printf("Testing terminate functionality: ");
    PriorityQueueT pq;
    priority_queue_create(&pq, 2);
    
    // Add some items
    priority_queue_push(&pq, 0, 100);
    priority_queue_push(&pq, 1, 200);
    
    // Terminate the queue
    priority_queue_terminate(&pq);
    
    // Push after terminate should be ignored
    priority_queue_push(&pq, 0, 999);
    
    // Pop after terminate should fail
    unsigned int value;
    int result = priority_queue_pop(&pq, &value);
    assert(result != 0); // Should fail due to termination
    
    priority_queue_destroy(&pq);
    printf("passed\n");
}

void test_mixed_operations() {
    printf("Testing mixed operations: ");
    PriorityQueueT pq;
    priority_queue_create(&pq, 3);
    
    // Mix of pushes and pops
    priority_queue_push(&pq, 1, 100);
    priority_queue_push(&pq, 0, 50); // Higher priority
    
    unsigned int value;
    int result = priority_queue_pop(&pq, &value);
    assert(result == 0 && value == 50); // Higher priority first
    
    priority_queue_push(&pq, 2, 200); // Lower priority
    priority_queue_push(&pq, 0, 60);  // Higher priority again
    
    result = priority_queue_pop(&pq, &value);
    assert(result == 0 && value == 60); // Newest high priority
    
    result = priority_queue_pop(&pq, &value);
    assert(result == 0 && value == 100); // Original medium priority
    
    result = priority_queue_pop(&pq, &value);
    assert(result == 0 && value == 200); // Low priority last
    
    assert(priority_queue_empty(&pq));
    
    priority_queue_destroy(&pq);
    printf("test passed\n");
}


int main() {
    printf("Running priority queue tests...\n\n");
    
    test_create_destroy();
    test_basic_push_pop();
    test_priority_ordering();
    test_fifo_within_priority();
    test_empty_pop();
    test_terminate();
    test_mixed_operations();
    
    printf("\n All priority queue tests passed!\n");
  return 0;
}

