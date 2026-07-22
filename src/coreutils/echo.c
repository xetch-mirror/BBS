#include "init/include/sys_io.h"

void core_echo(const char *args) {
    if (args && *args) {
        io_print(args);
    }
    io_print("\n");
}
