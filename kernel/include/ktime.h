#ifndef KTIME_H
#define KTIME_H

#include "Clib/Xlibary/Xbool.h"

typedef struct {
    unsigned int seconds;
    unsigned int microseconds; // 0 - 999999
} ktime_t;

// Reads current hardware time / tick count
extern ktime_t ktime_get(void);

// Formats seconds and microseconds into a fixed 14-char buffer: "[  1.005227] "
static inline void ktime_format(ktime_t t, char *out_buf) {
    // Fill buffer template
    out_buf[0]  = '[';
    out_buf[1]  = ' ';
    out_buf[2]  = ' ';
    
    // Seconds (00 - 99 range padding)
    out_buf[3]  = '0' + ((t.seconds / 10) % 10);
    out_buf[4]  = '0' + (t.seconds % 10);
    out_buf[5]  = '.';

    // Microseconds padded to 6 digits
    out_buf[6]  = '0' + ((t.microseconds / 100000) % 10);
    out_buf[7]  = '0' + ((t.microseconds / 10000) % 10);
    out_buf[8]  = '0' + ((t.microseconds / 1000) % 10);
    out_buf[9]  = '0' + ((t.microseconds / 100) % 10);
    out_buf[10] = '0' + ((t.microseconds / 10) % 10);
    out_buf[11] = '0' + (t.microseconds % 10);
    out_buf[12] = ']';
    out_buf[13] = ' ';
    out_buf[14] = '\0';
}

#endif // KTIME_H
