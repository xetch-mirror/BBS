#include "init/include/sys_io.h"
#include "include/sys_string.h"
#include "include/sudolib.h"
#include "include/userlib.h"

extern void core_chroot(const char *path);
extern void core_cat(const char *filename);
extern void core_ls(const char *args);

void core_sudo(const char *cmd, const char *arg, const char *password) {
    user_profile_t *current = userlib_get_current_user();

    if (!password || *password == '\0') {
        io_print("usage: sudo <command> <arg> <password>\n");
        return;
    }

    // elevation
    if (sudolib_elevate(password) != 0) {
        return; // failed
    }

    io_print("[sudo] elevated session granted (UID: 0)\n");

    // execute privileged command
    if (k_strcmp(cmd, "chroot") == 0) {
        core_chroot(arg);
    } else if (k_strcmp(cmd, "cat") == 0) {
        core_cat(arg);
    } else if (k_strcmp(cmd, "ls") == 0) {
        core_ls(arg);
    } else {
        io_print("[sudo] unknown privileged command: ");
        io_print(cmd);
        io_print("\n");
    }

    // hacked?
    sudolib_drop_privileges();
    io_print("[sudo] privileges dropped (UID: ");
    char buf[16];
    print(userlib_get_current_user()->uid, buf, 10);
    io_print(buf);
    io_print(")\n");
}
