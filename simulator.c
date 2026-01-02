#include "simulator.h"
#include "list.h"
#include "non_blocking_queue.h"
#include "blocking_queue.h"
#include "utilities.h"
#include "logger.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

// Global variables for thread management
static pthread_t* threads = NULL;
static int thread_count = 0;
static int* thread_ids = NULL;

// Global variables for process ID management
static BlockingQueueT process_id_pool;
static int max_processes = 0;

// Process management structures
typedef struct Process {
    ProcessIdT pid;
    ProcessStateT state;
    EvaluatorCodeT code;
    unsigned int PC;                // Program counter
    unsigned int cpu_time;          // CPU time used
    sem_t wait_semaphore;           // Semaphore for waiting threads
    int waiting_threads;            // Number of threads waiting for this process
} ProcessT;

static ProcessT* process_table = NULL;
static PriorityQueueT ready_queue;
static PriorityT max_priority_level = 0;
static pthread_mutex_t process_table_mutex = PTHREAD_MUTEX_INITIALIZER;
static int simulator_running = 0;

// Thread routine function
void* simulator_routine(void* arg) {
    int thread_id = *(int*)arg;
    
    // Log thread start message
    char message[100];
    snprintf(message, sizeof(message), "Simulator thread %d started", thread_id);
    logger_write(message);
    
    // Main scheduling loop
    while (simulator_running) {
        unsigned int pid_value;
        
        // Try to get a process from the ready queue (non-blocking)
        int result = priority_queue_pop(&ready_queue, &pid_value);
        if (result != 0) {
            // No processes available, sleep briefly and continue
            usleep(1000); // Sleep for 1ms
            continue;
        }
        
        ProcessIdT pid = (ProcessIdT)pid_value;
        
        pthread_mutex_lock(&process_table_mutex);
        
        // Check if process is still valid and ready
        if (process_table[pid].state != ready) {
            pthread_mutex_unlock(&process_table_mutex);
            continue;
        }
        
        // Mark process as running
        process_table[pid].state = running;
        
        // Get process info for execution
        EvaluatorCodeT code = process_table[pid].code;
        unsigned int PC = process_table[pid].PC;
        
        pthread_mutex_unlock(&process_table_mutex);
        
        // Run the process using the evaluator
        EvaluatorResultT result_eval = evaluator_evaluate(code, PC);
        
        pthread_mutex_lock(&process_table_mutex);
        
        // Update process state based on result
        process_table[pid].PC = result_eval.PC;
        process_table[pid].cpu_time += result_eval.cpu_time;
        
        switch (result_eval.reason) {
            case reason_terminated:
                process_table[pid].state = terminated;
                // Wake up any threads waiting for this process using semaphore
                // Post semaphore for each waiting thread
                for (int i = 0; i < process_table[pid].waiting_threads; i++) {
                    sem_post(&process_table[pid].wait_semaphore);
                }
                snprintf(message, sizeof(message), "Process %u terminated", pid);
                logger_write(message);
                break;
                
            case reason_timeslice_ended:
                // Put process back in ready queue for round-robin
                process_table[pid].state = ready;
                priority_queue_push(&ready_queue, 0, pid);
                break;
                
            case reason_blocked:
                // Process is blocked, don't put back in ready queue
                process_table[pid].state = blocked;
                break;
        }
        
        pthread_mutex_unlock(&process_table_mutex);
    }
    
    snprintf(message, sizeof(message), "Simulator thread %d terminated", thread_id);
    logger_write(message);
    return NULL;
}

// Process ID management functions
ProcessIdT allocate_process_id() {
    unsigned int pid;
    int result = blocking_queue_pop(&process_id_pool, &pid);
    if (result != 0) {
        // This should not happen unless the queue is terminated
        logger_write("Failed to allocate process ID");
        return 0;
    }
    return (ProcessIdT)pid;
}

void deallocate_process_id(ProcessIdT pid) {
    blocking_queue_push(&process_id_pool, (unsigned int)pid);
}

void simulator_start(int thread_count_param, int max_processes_param, PriorityT max_priority_level_param) {
    thread_count = thread_count_param;
    max_processes = max_processes_param;
    max_priority_level = max_priority_level_param;
    simulator_running = 1;
    
    // Initialize process ID pool
    blocking_queue_create(&process_id_pool);
    
    // Add all available process IDs to the pool (0 to max_processes-1)
    for (int i = 0; i < max_processes; i++) {
        blocking_queue_push(&process_id_pool, (unsigned int)i);
    }
    
    logger_write("Process ID pool initialized");
    
    // Initialize process table
    process_table = checked_malloc(sizeof(ProcessT) * max_processes);
    for (int i = 0; i < max_processes; i++) {
        process_table[i].state = unallocated;
        process_table[i].waiting_threads = 0;
    }
    
    // Initialize ready queue with max_priority_level + 1 priority levels
    priority_queue_create(&ready_queue, max_priority_level + 1);
    
    logger_write("Process table and ready queue initialized");
    
    // Allocate memory for thread array
    threads = checked_malloc(sizeof(pthread_t) * thread_count);
    
    // Create array to hold thread IDs (need persistent storage for each thread)
    thread_ids = checked_malloc(sizeof(int) * thread_count);
    
    // Create threads
    for (int i = 0; i < thread_count; i++) {
        thread_ids[i] = i;
        int result = pthread_create(&threads[i], NULL, simulator_routine, &thread_ids[i]);
        if (result != 0) {
            logger_write("Failed to create simulator thread");
            exit(1);
        }
    }
}

void simulator_stop() {
    // Stop the simulator
    simulator_running = 0;
    
    // Terminate the ready queue to wake up any blocked threads
    priority_queue_terminate(&ready_queue);
    
    if (threads != NULL && thread_count > 0) {
        // Wait for all threads to finish
        for (int i = 0; i < thread_count; i++) {
            int result = pthread_join(threads[i], NULL);
            if (result != 0) {
                logger_write("Failed to join simulator thread");
            }
        }
        
        // Clean up allocated memory
        checked_free(threads);
        threads = NULL;
        
        if (thread_ids != NULL) {
            checked_free(thread_ids);
            thread_ids = NULL;
        }
        
        thread_count = 0;
        
        logger_write("All simulator threads terminated");
    }
    
    // Clean up process table
    if (process_table != NULL) {
        for (int i = 0; i < max_processes; i++) {
            if (process_table[i].state != unallocated) {
                sem_destroy(&process_table[i].wait_semaphore);
            }
        }
        checked_free(process_table);
        process_table = NULL;
        logger_write("Process table destroyed");
    }
    
    // Clean up ready queue
    priority_queue_destroy(&ready_queue);
    logger_write("Ready queue destroyed");
    
    // Clean up process ID pool
    if (max_processes > 0) {
        blocking_queue_destroy(&process_id_pool);
        max_processes = 0;
        logger_write("Process ID pool destroyed");
    }
}

ProcessIdT simulator_create_process(EvaluatorCodeT const code) {
    // Allocate a process ID
    ProcessIdT pid = allocate_process_id();
    
    pthread_mutex_lock(&process_table_mutex);
    
    // Initialize process in process table
    process_table[pid].pid = pid;
    process_table[pid].state = ready;
    process_table[pid].code = code;
    process_table[pid].PC = 0;
    process_table[pid].cpu_time = 0;
    sem_init(&process_table[pid].wait_semaphore, 0, 0); // Initialize semaphore with 0 count
    process_table[pid].waiting_threads = 0;
    
    // Add to ready queue with default priority (0 = highest)
    priority_queue_push(&ready_queue, 0, pid);
    
    pthread_mutex_unlock(&process_table_mutex);
    
    // Log process creation
    char message[100];
    snprintf(message, sizeof(message), "Process %u created", pid);
    logger_write(message);
    
    return pid;
}

void simulator_wait(ProcessIdT pid) {
    // Log that we're waiting for this process
    char message[100];
    snprintf(message, sizeof(message), "Waiting for process %u", pid);
    logger_write(message);
    
    pthread_mutex_lock(&process_table_mutex);
    
    // Check if process is already terminated
    if (process_table[pid].state == terminated) {
        pthread_mutex_unlock(&process_table_mutex);
        return;
    }
    
    // Increment waiting threads count
    process_table[pid].waiting_threads++;
    pthread_mutex_unlock(&process_table_mutex);
    
    // Wait on semaphore until process terminates
    sem_wait(&process_table[pid].wait_semaphore);
    
    pthread_mutex_lock(&process_table_mutex);
    process_table[pid].waiting_threads--;
    
    // Clean up process data if no more threads are waiting
    if (process_table[pid].waiting_threads == 0) {
        sem_destroy(&process_table[pid].wait_semaphore);
        process_table[pid].state = unallocated;
        
        // Recycle the process ID
        deallocate_process_id(pid);
        
        snprintf(message, sizeof(message), "Process %u terminated and cleaned up", pid);
        logger_write(message);
    }
    
    pthread_mutex_unlock(&process_table_mutex);
}

void simulator_kill(ProcessIdT pid) {
}

void simulator_event() {
}
