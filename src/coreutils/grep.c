#include "init/include/sys_io.h"
#include "include/sys_string.h"

void core_grep(const char *pattern, const char *text) {
    if (!pattern || !text || *pattern == '\0') {
        io_print("grep: missing pattern or text\n");
        return;
    }

    // substring
    if (k_strstr(text, pattern)) {
        io_print(text);
        io_print("\n");
    }
}
