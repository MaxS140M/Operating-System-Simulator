#ifndef _BOOSTER_DAEMON_H_
#define _BOOSTER_DAEMON_H_

#include <unistd.h>

void booster_daemon_start(useconds_t interval);
void booster_daemon_stop();

#endif
