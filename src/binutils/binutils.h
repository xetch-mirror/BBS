#ifndef BINUTILS_H
#define BINUTILS_H

#include <stdint.h>

void pkg_init(void);

void app_pkg(const char *action, const char *pkg_name);

void app_mininano(const char *filename);

void pkg_list(void);
void pkg_install(const char *pkg_name);

#endif // BINUTILS_H
