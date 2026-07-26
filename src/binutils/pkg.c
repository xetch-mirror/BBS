#include "init/include/sys_io.h"
#include "kshell/sys_string.h"
#include "binutils.h"
#include "fs/vfs.h"

#define MAX_PACKAGES 32

typedef struct {
    char name[32];
    char version[16];
    char binary_path[64]; // e.g., /usr/bin/mininano
    uint8_t installed;
} pkg_meta_t;

static pkg_meta_t pkg_db[MAX_PACKAGES];
static uint32_t pkg_count = 0;

void pkg_init(void) {
    // seed default package directly into /usr/bin/
    k_strncpy(pkg_db[0].name, "mininano", 32);
    k_strncpy(pkg_db[0].version, "1.0.0", 16);
    k_strncpy(pkg_db[0].binary_path, "/usr/bin/mininano", 64);
    pkg_db[0].installed = 1;

    pkg_count = 1;
}

void pkg_list(void) {
    io_print("Installed BBUR Packages:\n");
    io_print("--------------------------------------------------\n");
    for (uint32_t i = 0; i < pkg_count; i++) {
        if (pkg_db[i].installed) {
            io_print(" [i] ");
            io_print(pkg_db[i].name);
            io_print(" (v");
            io_print(pkg_db[i].version);
            io_print(") -> ");
            io_print(pkg_db[i].binary_path);
            io_print("\n");
        }
    }
}

void pkg_install(const char *pkg_name) {
    io_print("[pkg] fetching manifest from /disk/sda1/");
    io_print(pkg_name);
    io_print(".bbur...\n");

    for (uint32_t i = 0; i < pkg_count; i++) {
        if (k_strcmp(pkg_db[i].name, pkg_name) == 0) {
            if (pkg_db[i].installed) {
                io_print("[pkg] ");
                io_print(pkg_name);
                io_print(" is already installed in /usr/bin/\n");
                return;
            }
            
            // vinary installs directly to /usr/bin/
            char bin_target[64] = "/usr/bin/";
            k_strcat(bin_target, pkg_name);
            
            k_strncpy(pkg_db[i].binary_path, bin_target, 64);
            pkg_db[i].installed = 1;

            io_print("[pkg] successfully installed ");
            io_print(pkg_name);
            io_print(" to ");
            io_print(bin_target);
            io_print("!\n");
            return;
        }
    }

    io_print("[pkg] Error: package '");
    io_print(pkg_name);
    io_print("' not found.\n");
}

void app_pkg(const char *action, const char *pkg_name) {
    if (!action || k_strcmp(action, "list") == 0) {
        pkg_list();
    } else if (k_strcmp(action, "install") == 0) {
        if (!pkg_name) {
            io_print("Usage: pkg install <package_name>\n");
            return;
        }
        pkg_install(pkg_name);
    } else {
        io_print("Usage: pkg [list|install <pkg>]\n");
    }
}
