#include "event_source.h"
#include "utilities.h"
#include "simulator.h"
#include "logger.h"

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

// Global variables for event source thread management
static pthread_t event_thread;
static int event_source_running = 0;
static useconds_t event_interval = 0;

// Event source thread routine
void* event_source_routine(void* arg) {
    logger_write("Event source thread started");
    
    while (event_source_running) {
        // Sleep for the specified interval
        usleep(event_interval);
        
        // Generate an event by calling simulator_event
        if (event_source_running) {  // Check again in case we're shutting down
            simulator_event();
        }
    }
    
    logger_write("Event source thread terminated");
    return NULL;
}

void event_source_start(useconds_t interval) {
    event_interval = interval;
    event_source_running = 1;
    
    char message[100];
    snprintf(message, sizeof(message), "Event source starting with interval %u microseconds", interval);
    logger_write(message);
    
    // Create the event source thread
    int result = pthread_create(&event_thread, NULL, event_source_routine, NULL);
    if (result != 0) {
        logger_write("Failed to create event source thread");
        exit(1);
    }
    
    logger_write("Event source started");
}

void event_source_stop() {
    if (event_source_running) {
        event_source_running = 0;
        
        // Wait for the event thread to finish
        int result = pthread_join(event_thread, NULL);
        if (result != 0) {
            logger_write("Failed to join event source thread");
        }
        
        logger_write("Event source stopped");
    }
}

