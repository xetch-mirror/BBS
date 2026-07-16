#ifndef XCONIO_H
#define XCONIO_H

// Clear the screen using your terminal's escape codes or a direct kernel call
static inline void clrscr(void) {
    // If your terminal supports standard ANSI escape codes:
    extern void kernel_print(const char* s);
    kernel_print("\033[2J\033[H"); 
}

// Move the cursor to a specific column (x) and row (y)
static inline void gotoxy(int x, int y) {
    // You can format a quick ANSI string or call a direct screen-coordinate system call
    // e.g., "\033[y;xH"
}

// Check if a keyboard key has been pressed without pausing the program (non-blocking)
static inline int kbhit(void) {
    extern int kernel_check_keyboard(void);
    return kernel_check_keyboard();
}

#endif // XCONIO_H
