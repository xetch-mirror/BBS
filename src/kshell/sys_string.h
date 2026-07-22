#ifndef SYS_STRING_H
#define SYS_STRING_H

#include <stdint.h>

int k_strcmp(const char *s1, const char *s2);
int k_strncmp(const char *s1, const char *s2, uint32_t n);

#endif // SYS_STRING_H
