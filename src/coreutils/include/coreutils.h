#ifndef COREUTILS_H
#define COREUTILS_H

void core_echo(const char *args);
void core_ls(const char *args);
void core_cat(const char *filename);
void core_grep(const char *pattern, const char *text);
void core_chroot(const char *path);

#endif // COREUTILS_H
