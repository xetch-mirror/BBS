#include "Clib/Xlibary/xio.h"
#include "Clib/Xlibary/xstring.h"

#define COLOR_RESET   "\033[0m"
#define COLOR_BOLD    "\033[1m"
#define COLOR_HEADER  "\033[1;33m"
#define COLOR_CODE    "\033[36m"
#define COLOR_QUOTE   "\033[32m"

void markdown_render_line(const char *line) {
    if (line[0] == '<') return;

    if (line[0] == '>' && line[1] == ' ') {
        kernel_print(COLOR_QUOTE);
        kernel_print("│ ");
        kernel_print(line + 2);
        kernel_print(COLOR_RESET);
        kernel_print("\n");
        return;
    }

    if (line[0] == '#') {
        int level = 0;
        while (line[level] == '#') level++;
        if (line[level] == ' ') {
            kernel_print(COLOR_HEADER);
            kernel_print(line + level + 1);
            kernel_print(COLOR_RESET);
            kernel_print("\n");
            return;
        }
    }

    if ((line[0] == '*' || line[0] == '-') && line[1] == ' ') {
        kernel_print("  • ");
        line += 2;
    }

    const char *p = line;
    int bold = 0, code = 0;

    while (*p) {
        if (*p == '<') {
            while (*p && *p != '>') p++;
            if (*p == '>') p++;
            continue;
        }

        if (*p == '*' && *(p + 1) == '*') {
            bold = !bold;
            kernel_print(bold ? COLOR_BOLD : COLOR_RESET);
            p += 2;
            continue;
        }

        if (*p == '`') {
            code = !code;
            kernel_print(code ? COLOR_CODE : COLOR_RESET);
            p++;
            continue;
        }

        char buf[2] = {*p, '\0'};
        kernel_print(buf);
        p++;
    }
    kernel_print(COLOR_RESET);
    kernel_print("\n");
}
