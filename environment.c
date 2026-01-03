#include "environment.h"
#include "simulator.h"
#include "utilities.h"
#include "evaluator.h"
#include "list.h"
#include "logger.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Global variables for thread management
static pthread_t* threads = NULL;
static pthread_t* blocking_threads = NULL;
static int thread_count = 0;
static int* thread_ids = NULL;
static int* blocking_thread_ids = NULL;
static unsigned int iterations = 0;
static unsigned int batch_size = 0;

// Thread routine function
void* terminating_routine(void* arg) {
    int thread_id = *(int*)arg;
    
    // Log thread start message
    char message[200];
    snprintf(message, sizeof(message), "Environment thread %d started", thread_id);
    logger_write(message);
    
    // Loop for the specified number of iterations
    for (unsigned int iter = 0; iter < iterations; iter++) {
        ProcessIdT* process_ids = checked_malloc(sizeof(ProcessIdT) * batch_size);
        
        // Create batch_size processes
        for (unsigned int i = 0; i < batch_size; i++) {
            // Generate random number of steps between 1 and 20
            unsigned int steps = (rand() % 20) + 1;
            
            EvaluatorCodeT code;
            // Create a mix of blocking and non-blocking processes (50/50 split)
            if (rand() % 2 == 0) {
                code = evaluator_terminates_after(steps);
            } else {
                code = evaluator_blocking_terminates_after(steps);
            }
            
            process_ids[i] = simulator_create_process(code);
            
            snprintf(message, sizeof(message), 
                    "Environment thread %d created process %u (iteration %u, process %u)", 
                    thread_id, process_ids[i], iter, i);
            logger_write(message);
        }
        
        // Wait for each process to finish
        for (unsigned int i = 0; i < batch_size; i++) {
            simulator_wait(process_ids[i]);
            snprintf(message, sizeof(message), 
                    "Environment thread %d waited for process %u (iteration %u, process %u)", 
                    thread_id, process_ids[i], iter, i);
            logger_write(message);
        }
        
        checked_free(process_ids);
        
        snprintf(message, sizeof(message), 
                "Environment thread %d completed iteration %u", thread_id, iter);
        logger_write(message);
    }
    
    snprintf(message, sizeof(message), "Environment thread %d terminated", thread_id);
    logger_write(message);
    return NULL;
}

// Thread routine function for blocking processes
void* blocking_routine(void* arg) {
    int thread_id = *(int*)arg;
    
    // Log thread start message
    char message[200];
    snprintf(message, sizeof(message), "Environment blocking thread %d started", thread_id);
    logger_write(message);
    
    // Loop for the specified number of iterations
    for (unsigned int iter = 0; iter < iterations; iter++) {
        ProcessIdT* process_ids = checked_malloc(sizeof(ProcessIdT) * batch_size);
        
        // Create batch_size blocking processes that terminate after 5 steps
        for (unsigned int i = 0; i < batch_size; i++) {
            EvaluatorCodeT code = evaluator_blocking_terminates_after(5);
            process_ids[i] = simulator_create_process(code);
            
            snprintf(message, sizeof(message), 
                    "Environment blocking thread %d created blocking process %u (iteration %u, process %u)", 
                    thread_id, process_ids[i], iter, i);
            logger_write(message);
        }
        
        // Wait for each process to finish
        for (unsigned int i = 0; i < batch_size; i++) {
            simulator_wait(process_ids[i]);
            snprintf(message, sizeof(message), 
                    "Environment blocking thread %d waited for blocking process %u (iteration %u, process %u)", 
                    thread_id, process_ids[i], iter, i);
            logger_write(message);
        }
        
        checked_free(process_ids);
        
        snprintf(message, sizeof(message), 
                "Environment blocking thread %d completed iteration %u", thread_id, iter);
        logger_write(message);
    }
    
    snprintf(message, sizeof(message), "Environment blocking thread %d terminated", thread_id);
    logger_write(message);
    return NULL;
}

void environment_start(unsigned int thread_count_param,
		       unsigned int iterations_param,
		       unsigned int batch_size_param) {
    thread_count = thread_count_param;
    iterations = iterations_param;
    batch_size = batch_size_param;
    
    // Initialize random number generator
    srand((unsigned int)time(NULL));
    
    logger_write("Environment starting");
    
    // Allocate memory for thread array
    threads = checked_malloc(sizeof(pthread_t) * thread_count);
    blocking_threads = checked_malloc(sizeof(pthread_t) * thread_count);
    
    // Create array to hold thread IDs (need persistent storage for each thread)
    thread_ids = checked_malloc(sizeof(int) * thread_count);
    blocking_thread_ids = checked_malloc(sizeof(int) * thread_count);
    
    // Create terminating threads
    for (int i = 0; i < thread_count; i++) {
        thread_ids[i] = i;
        int result = pthread_create(&threads[i], NULL, terminating_routine, &thread_ids[i]);
        if (result != 0) {
            logger_write("Failed to create environment thread");
            exit(1);
        }
    }
    
    // Create blocking threads
    for (int i = 0; i < thread_count; i++) {
        blocking_thread_ids[i] = i;
        int result = pthread_create(&blocking_threads[i], NULL, blocking_routine, &blocking_thread_ids[i]);
        if (result != 0) {
            logger_write("Failed to create environment blocking thread");
            exit(1);
        }
    }
    
    char message[150];
    snprintf(message, sizeof(message), "Environment started with %d terminating threads and %d blocking threads, %u iterations, %u batch size", 
             thread_count, thread_count, iterations, batch_size);
    logger_write(message);
}

void environment_stop() {
    if (threads != NULL && thread_count > 0) {
        // Wait for all terminating threads to finish
        for (int i = 0; i < thread_count; i++) {
            int result = pthread_join(threads[i], NULL);
            if (result != 0) {
                logger_write("Failed to join environment thread");
            }
        }
        
        // Wait for all blocking threads to finish
        for (int i = 0; i < thread_count; i++) {
            int result = pthread_join(blocking_threads[i], NULL);
            if (result != 0) {
                logger_write("Failed to join environment blocking thread");
            }
        }
        
        // Clean up allocated memory
        checked_free(threads);
        threads = NULL;
        
        checked_free(blocking_threads);
        blocking_threads = NULL;
        
        if (thread_ids != NULL) {
            checked_free(thread_ids);
            thread_ids = NULL;
        }
        
        if (blocking_thread_ids != NULL) {
            checked_free(blocking_thread_ids);
            blocking_thread_ids = NULL;
        }
        
        thread_count = 0;
        
        logger_write("All environment threads terminated");
    }
    
    logger_write("Environment stopped");
}
