#include "non_blocking_queue.h"
#include "utilities.h"

#include <assert.h>
#include <stdio.h>
#include <limits.h>

void test_create_destroy() {
    printf("Testing create and destroy: ");
    NonBlockingQueueT queue;
    non_blocking_queue_create(&queue);
    assert(non_blocking_queue_empty(&queue));
    assert(non_blocking_queue_length(&queue) == 0);
    non_blocking_queue_destroy(&queue);
    printf("test passed\n");
}

void test_empty_queue_operations() {
    printf("Testing empty queue operations: ");
    NonBlockingQueueT queue;
    non_blocking_queue_create(&queue);
    
    // Test empty queue properties
    assert(non_blocking_queue_empty(&queue));
    assert(non_blocking_queue_length(&queue) == 0);
    
    // Test pop on empty queue should fail
    unsigned int value;
    int result = non_blocking_queue_pop(&queue, &value);
    assert(result != 0); // Should return non-zero (failure)
    
    non_blocking_queue_destroy(&queue);
    printf("test passed\n");
}

void test_single_element() {
    printf("Testing single element operations: ");
    NonBlockingQueueT queue;
    non_blocking_queue_create(&queue);
    
    // Push one element
    non_blocking_queue_push(&queue, 42);
    assert(!non_blocking_queue_empty(&queue));
    assert(non_blocking_queue_length(&queue) == 1);
    
    // Pop the element
    unsigned int value;
    int result = non_blocking_queue_pop(&queue, &value);
    assert(result == 0); // Should succeed
    assert(value == 42);
    
    // Queue should be empty again
    assert(non_blocking_queue_empty(&queue));
    assert(non_blocking_queue_length(&queue) == 0);
    
    non_blocking_queue_destroy(&queue);
    printf("test passed\n");
}

void test_multiple_elements() {
    printf("Testing multiple elements (FIFO behavior): ");
    NonBlockingQueueT queue;
    non_blocking_queue_create(&queue);
    
    // Push multiple elements
    unsigned int test_values[] = {10, 20, 30, 40, 50};
    int num_values = sizeof(test_values) / sizeof(test_values[0]);
    
    for (int i = 0; i < num_values; i++) {
        non_blocking_queue_push(&queue, test_values[i]);
        assert(non_blocking_queue_length(&queue) == i + 1);
    }
    
    assert(!non_blocking_queue_empty(&queue));
    assert(non_blocking_queue_length(&queue) == num_values);
    
    // Pop elements and verify FIFO order
    for (int i = 0; i < num_values; i++) {
        unsigned int value;
        int result = non_blocking_queue_pop(&queue, &value);
        assert(result == 0); // Should succeed
        assert(value == test_values[i]); // FIFO order
        assert(non_blocking_queue_length(&queue) == num_values - i - 1);
    }
    
    assert(non_blocking_queue_empty(&queue));
    assert(non_blocking_queue_length(&queue) == 0);
    
    non_blocking_queue_destroy(&queue);
    printf("test passed\n");
}

void test_mixed_operations() {
    printf("Testing mixed push/pop operations: ");
    NonBlockingQueueT queue;
    non_blocking_queue_create(&queue);
    
    // Mix of push and pop operations
    non_blocking_queue_push(&queue, 100);
    non_blocking_queue_push(&queue, 200);
    assert(non_blocking_queue_length(&queue) == 2);
    
    unsigned int value;
    int result = non_blocking_queue_pop(&queue, &value);
    assert(result == 0 && value == 100);
    assert(non_blocking_queue_length(&queue) == 1);
    
    non_blocking_queue_push(&queue, 300);
    assert(non_blocking_queue_length(&queue) == 2);
    
    result = non_blocking_queue_pop(&queue, &value);
    assert(result == 0 && value == 200);
    
    result = non_blocking_queue_pop(&queue, &value);
    assert(result == 0 && value == 300);
    
    assert(non_blocking_queue_empty(&queue));
    
    // Try to pop from empty queue
    result = non_blocking_queue_pop(&queue, &value);
    assert(result != 0); // Should fail
    
    non_blocking_queue_destroy(&queue);
    printf("test passed\n");
}

void test_edge_values() {
    printf("Testing edge values: ");
    NonBlockingQueueT queue;
    non_blocking_queue_create(&queue);
    
    // Test with edge values for unsigned int
    unsigned int edge_values[] = {0, 1, UINT_MAX - 1, UINT_MAX};
    int num_values = sizeof(edge_values) / sizeof(edge_values[0]);
    
    for (int i = 0; i < num_values; i++) {
        non_blocking_queue_push(&queue, edge_values[i]);
    }
    
    for (int i = 0; i < num_values; i++) {
        unsigned int value;
        int result = non_blocking_queue_pop(&queue, &value);
        assert(result == 0);
        assert(value == edge_values[i]);
    }
    
    assert(non_blocking_queue_empty(&queue));
    non_blocking_queue_destroy(&queue);
    printf("test passed\n");
}

int main() {
    printf("Running non-blocking queue tests...\n\n");
    
    test_create_destroy();
    test_empty_queue_operations();
    test_single_element();
    test_multiple_elements();
    test_mixed_operations();
    test_edge_values();
    
    printf("\nAll non-blocking queue tests passed!\n");
    return 0;
}
