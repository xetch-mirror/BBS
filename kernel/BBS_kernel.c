#include <stdint.h>

extern void kmain(void);

void BBS_kernel(void) {
    
    kmain();
 {
        __asm__ volatile("cli; hlt");
    }
}
 