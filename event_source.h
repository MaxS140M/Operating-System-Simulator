#ifndef _EVENT_SOURCE_H_
#define _EVENT_SOURCE_H_

#include <unistd.h>

void event_source_start(useconds_t interval);
void event_source_stop();

#endif
