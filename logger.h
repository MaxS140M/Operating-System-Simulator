#ifndef _LOGGER_H_
#define _LOGGER_H_

void logger_start(int logger_stream, int debug_stream, int priority_stream);
void logger_stop();
void logger_write(char const* message);
void debug_write(char const* message);
void priority_write(char const* message);

#endif
