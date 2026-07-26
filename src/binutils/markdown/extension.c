#include "Clib/Xlibary/xbool.h"
#include "Clib/Xlibary/xmemory.h"
#include "Clib/Xlibary/xstring.h"

void markdown_render_line(const char *line);

typedef struct {
    const char *name;
    void (*fn)(const char *);
} MppNativeFunc;

void mpp_extension_render(const char *text) {
    if (text == 0) return;

    char buffer[512];
    const char *start = text;
    const char *p = text;

    while (*p) {
        if (*p == '\n') {
            int len = (int)(p - start);
            if (len >= (int)sizeof(buffer)) len = sizeof(buffer) - 1;

            Xmemcpy(buffer, start, len);
            buffer[len] = '\0';

            markdown_render_line(buffer);
            start = p + 1;
        }
        p++;
    }

    if (*start) {
        int len = (int)(p - start);
        if (len >= (int)sizeof(buffer)) len = sizeof(buffer) - 1;

        Xmemcpy(buffer, start, len);
        buffer[len] = '\0';

        markdown_render_line(buffer);
    }
}

static const MppNativeFunc mpp_exports[] = {
    {"markdown_render", mpp_extension_render},
    {0, 0}
};

const MppNativeFunc* mpp_get_extensions(void) {
    return mpp_exports;
}
