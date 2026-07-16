#ifndef ARGUMENT_H
#define ARGUMENT_H

// argument.h
// this is part of the Xlibary, everyone can use it

// provided by kernel's environment / linker
extern void kernel_print(const char* s);
extern void kernel_exit(int code);

static inline void print_spacer(void)  { kernel_print("  -> "); }
static inline void print_newline(void) { kernel_print("\n"); }
static inline int is_null(const char* str)   { return str == 0; }
static inline int is_empty(const char* str)  { return str[0] == '\0'; }
static inline int check_args(int argc)       { return argc > 1; }

static inline void handle_error(void) {
    kernel_print("error: no function provided, exiting\n");
    kernel_exit(1);
}

static inline void output_message(const char* msg) {
    print_spacer();
    kernel_print(msg);
    print_newline();
}

// anyone can call this function from their own main entry point
static inline void process_command(int argc, char* argv[]) {
    if (!check_args(argc) || is_null(argv[1]) || is_empty(argv[1])) {
        handle_error();
    }
    output_message(argv[1]);
}

#endif // ARGUMENT_H
