#include "init/include/sys_io.h"

// External hook
extern void carry_status(void);

void core_ls(const char *args) {
    (void)args; // warning you
    carry_status();
}
