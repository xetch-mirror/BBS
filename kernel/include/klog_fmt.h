#ifndef KLOG_FMT_H
#define KLOG_FMT_H

#include "Clib/Xbool.h"
#include "Clib/Xio.h"
#include "ktime.h"
#include "ksubsys.h"

// Low-level serial hook
extern void serial_write(const char *buf);

// Primary logger function matching Linux output style:
// Output: [  1.005227] serio: i8042 KBD port at 0x60,0x64 irq 1
static inline void pr_log(const ksubsys_t *subsys, const char *msg) {
    char time_str[16];
    
    // 1. Fetch current time and format bracket timestamp
    ktime_t now = ktime_get();
    ktime_format(now, time_str);

    // 2. Output timestamp
    serial_write(time_str);
    kernel_print(time_str);

    // 3. Output subsystem tag (if defined)
    if (subsys != 0 && subsys->tag != 0) {
        serial_write(subsys->tag);
        kernel_print(subsys->tag);
        serial_write(": ");
        kernel_print(": ");
    }

    // 4. Output message and newline
    if (msg != 0) {
        serial_write(msg);
        kernel_print(msg);
    }
    
    serial_write("\n");
    print_newline();
}

#endif // KLOG_FMT_H
