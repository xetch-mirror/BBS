#ifndef PASSWDLIB_H
#define PASSWDLIB_H

#include <stdint.h>

#define MAX_ENTRIES 16
#define MAX_LINE_LEN 128

typedef struct {
    char username[32];
    uint32_t uid;
    uint32_t gid;
    char home[64];
    char shell[32];
} passwd_entry_t;

typedef struct {
    char username[32];
    uint32_t pass_hash;
} shadow_entry_t;

// apis
void passwdlib_init(void);
int  passwdlib_lookup_uid(const char *username);
int  passwdlib_verify_password(const char *username, const char *password);
int  passwdlib_update_password(const char *username, const char *new_pass);

#endif // PASSWDLIB_H
