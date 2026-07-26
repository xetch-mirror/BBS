#ifndef XIO_H
#define XIO_H

// overlap
extern void kernel_print(const char* s);
extern int kernel_read_kbd(char* buf, int max_len);

// 100%
static inline void print(const char* s) {
    if (s != 0) {
        kernel_print(s);
    }
}

static inline void print_newline(void) {
    kernel_print("\n");
}

static inline void print_char(char c) {
    char buf[2] = {c, 0};
    kernel_print(buf);
}

#endif // XIO_H
