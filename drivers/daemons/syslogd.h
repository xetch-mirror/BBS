#ifndef SYSLOGD_H
#define SYSLOGD_H

#include "kernel/include/sys_types.h"

// Log severity levels
#define LOG_INFO    0
#define LOG_WARN    1
#define LOG_ERROR   2
#define LOG_DEBUG   3

typedef struct {
    uint8_t level;
    char origin[16];   // e.g. "mouse_d", "disk_d", "net_d"
    char message[128];
} log_event_t;

void syslogd_init(void);
void syslogd_log(uint8_t level, const char *origin, const char *msg);

#endif
