#ifndef SUDOLIB_H
#define SUDOLIB_H

#include <stdint.h>
#include "userlib.h"

#define MAX_SUDOERS 16

typedef struct {
    uint32_t uid;
    uint8_t allow_all;
    char restricted_chroot[64];
} sudo_rule_t;

// prototypes 
void sudolib_init(void);
int sudolib_add_rule(uint32_t uid, uint8_t allow_all, const char *allowed_chroot);
int sudolib_check_permission(uint32_t uid, const char *target_chroot);
int sudolib_elevate(const char *password);
void sudolib_drop_privileges(void);
int sudolib_is_elevated(void);

#endif // SUDOLIB_H
