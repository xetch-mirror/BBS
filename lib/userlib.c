#include "init/include/sys_io.h"
#include "include/sys_string.h"
#include "include/userlib.h"

static user_profile_t user_database[MAX_USERS];
static uint32_t total_users = 0;
static user_session_t current_session;

uint32_t userlib_hash_password(const char *pass) {
    uint32_t hash = 5381;
    int c;
    while ((c = *pass++)) {
        hash = ((hash << 5) + hash) + c; // djb2 algorithm
    }
    return hash;
}

void userlib_init(void) {
    total_users = 0;
    current_session.is_authenticated = 0;
    current_session.session_id = 1000;

    userlib_add_user("root", "root", ROLE_ROOT, "/");
    // Create Default Guest / Standard User (UID 1000)
    userlib_add_user("guest", "bbs123", ROLE_USER, "/home/guest");

    // random user 
    userlib_switch_context(1000);
}

int userlib_add_user(const char *name, const char *pass, user_role_t role, const char *root_path) {
    if (total_users >= MAX_USERS) return -1;

    user_profile_t *u = &user_database[total_users];
    u->uid = (role == ROLE_ROOT) ? 0 : (1000 + total_users);
    u->gid = u->uid;
    u->role = role;
    u->password_hash = userlib_hash_password(pass);

    // copy 
    uint32_t i = 0;
    while (name[i] && i < MAX_USERNAME - 1) { u->username[i] = name[i]; i++; }
    u->username[i] = '\0';

    i = 0;
    while (root_path[i] && i < 63) { u->chroot_path[i] = root_path[i]; u->home_dir[i] = root_path[i]; i++; }
    u->chroot_path[i] = '\0';
    u->home_dir[i] = '\0';

    total_users++;
    return u->uid;
}

user_profile_t* userlib_get_current_user(void) {
    return &current_session.current_user;
}

int userlib_authenticate(const char *username, const char *password) {
    uint32_t pass_hash = userlib_hash_password(password);

    for (uint32_t i = 0; i < total_users; i++) {
        if (k_strcmp(user_database[i].username, username) == 0) {
            if (user_database[i].password_hash == pass_hash) {
                current_session.current_user = user_database[i];
                current_session.is_authenticated = 1;
                return 0; // Success
            }
            break;
        }
    }
    return -1; // FAILED!
}

int userlib_switch_context(uint32_t uid) {
    for (uint32_t i = 0; i < total_users; i++) {
        if (user_database[i].uid == uid) {
            current_session.current_user = user_database[i];
            current_session.is_authenticated = (uid == 1000) ? 1 : current_session.is_authenticated;
            return 0;
        }
    }
    return -1;
}

void userlib_logout(void) {
    userlib_switch_context(1000); // std user
}
