//student: Max Sloam ID:20666789

#include "logger.h"
#include "utilities.h"

#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

pthread_mutex_t logger_mutex;
unsigned long long message_counter = 0;
int logger_enabled = 0;
int debug_enabled = 0;
int priority_enabled = 0;

static void write_message(const char* prefix, const char* message) {
    pthread_mutex_lock(&logger_mutex);
    
    // Get time
    time_t now;
    struct tm *local_time;
    char time_str[9];
    
    time(&now);
    local_time = localtime(&now);
    strftime(time_str, sizeof(time_str), "%H:%M:%S", local_time);
    
    // Print message
    printf("%llu : %s : [%s] %s\n", message_counter, time_str, prefix, message);
    fflush(stdout);
    
    message_counter++;
    pthread_mutex_unlock(&logger_mutex);
}

void logger_start(int logger_stream, int debug_stream, int priority_stream) {
    pthread_mutex_init(&logger_mutex, NULL);
    message_counter = 0;
    logger_enabled = logger_stream;
    debug_enabled = debug_stream;
    priority_enabled = priority_stream;
}

void logger_stop() {
    pthread_mutex_destroy(&logger_mutex);
}

void logger_write(char const* message) {
    if (logger_enabled) {
        write_message("Log", message);
    }
}

void debug_write(char const* message) {
    if (debug_enabled) {
        write_message("Debug", message);
    }
}

void priority_write(char const* message) {
    if (priority_enabled) {
        write_message("Priority", message);
    }
}
