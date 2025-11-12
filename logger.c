#include "logger.h"
#include "utilities.h"

#include <pthread.h>
#include <stdio.h>

pthread_mutex_t logger_mutex;
unsigned long long message_counter = 0;

void logger_start(int logger_stream, int debug_stream, int priority_stream) {
}

void logger_stop() {
}

void logger_write(char const* message) {
}

void debug_write(char const* message) {
}

void priority_write(char const* message) {
}
