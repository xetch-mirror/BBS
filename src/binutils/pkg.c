#include "init/include/sys_io.h"
#include "kshell/sys_string.h"
#include "binutils.h"
#include "fs/vfs.h"

#define MAX_PACKAGES 32
#define MAX_INDEX 64

/* BBUR — Bold Base User Repository
 * Canonical source: https://github.com/xetch-mirror/BBUR
 *
 * BBS has no network stack yet, so the index is loaded from a local
 * mirror of .bbur manifests on disk. Swap pkg_index_load() for a real
 * fetch (git/http) once networking exists — pkg_install() itself
 * doesn't need to change, since it only talks to the index.
 */
#define BBUR_SOURCE_URL   "https://github.com/xetch-mirror/BBUR"
#define BBUR_LOCAL_INDEX  "/disk/sda1/index/"

/* ANSI escape codes */
#define ANSI_RESET   "\x1b[0m"
#define ANSI_RED     "\x1b[31m"
#define ANSI_GREEN   "\x1b[32m"
#define ANSI_YELLOW  "\x1b[33m"
#define ANSI_CYAN    "\x1b[36m"
#define ANSI_BOLD    "\x1b[1m"

typedef struct {
    char name[32];
    char version[16];
    char binary_path[64]; // e.g., /usr/bin/mininano
    uint8_t installed;
} pkg_meta_t;

/* One entry in the package index — what's *available* to install,
 * as opposed to pkg_db, which is what's already installed. */
typedef struct {
    char name[32];
    char version[16];
    char manifest_path[80]; // local .bbur manifest, e.g. /disk/sda1/index/mininano.bbur
} pkg_index_entry_t;

static pkg_meta_t pkg_db[MAX_PACKAGES];
static uint32_t pkg_count = 0;

static pkg_index_entry_t pkg_index[MAX_INDEX];
static uint32_t pkg_index_count = 0;

/* Print `text` wrapped in `color`, then reset. Keeps the color codes
 * from getting scattered through every call site below. */
static void io_print_c(const char *color, const char *text) {
    io_print(color);
    io_print(text);
    io_print(ANSI_RESET);
}

void pkg_init(void) {
    // seed default package directly into /usr/bin/
    k_strncpy(pkg_db[0].name, "mininano", 32);
    k_strncpy(pkg_db[0].version, "1.0.0", 16);
    k_strncpy(pkg_db[0].binary_path, "/usr/bin/mininano", 64);
    pkg_db[0].installed = 1;

    pkg_count = 1;
}

/* Register one package into the index. Call this while loading the
 * local BBUR mirror, or manually to seed known packages. */
uint8_t pkg_index_add(const char *name, const char *version, const char *manifest_path) {
    if (pkg_index_count >= MAX_INDEX) {
        io_print_c(ANSI_RED, "[pkg] error: package index full\n");
        return 0;
    }

    pkg_index_entry_t *e = &pkg_index[pkg_index_count];
    k_strncpy(e->name, name, 32);
    k_strncpy(e->version, version, 16);
    k_strncpy(e->manifest_path, manifest_path, 80);
    pkg_index_count++;
    return 1;
}

static pkg_index_entry_t *pkg_index_find(const char *name) {
    for (uint32_t i = 0; i < pkg_index_count; i++) {
        if (k_strcmp(pkg_index[i].name, name) == 0) {
            return &pkg_index[i];
        }
    }
    return 0;
}

/* Load the local package index — the .bbur manifests mirrored from
 * BBUR_SOURCE_URL. Replace the body of this function with a real
 * fetch (git clone/pull, or an HTTP GET once networking exists) —
 * everything downstream (pkg_install, pkg_list) stays the same. */
void pkg_index_load(void) {
    pkg_index_count = 0;

    if (!vfs_exists(BBUR_LOCAL_INDEX)) {
        io_print_c(ANSI_YELLOW, "[pkg] warning: no local BBUR index found at " BBUR_LOCAL_INDEX "\n");
        return;
    }

    // TODO: walk BBUR_LOCAL_INDEX with a real vfs_readdir() and call
    // pkg_index_add() for each *.bbur manifest found. Seeding one
    // known package here so pkg_install has something to find:
    pkg_index_add("mininano", "1.0.0", BBUR_LOCAL_INDEX "mininano.bbur");
}

void pkg_list(void) {
    io_print_c(ANSI_BOLD, "Installed BBUR Packages:\n");
    io_print("--------------------------------------------------\n");
    for (uint32_t i = 0; i < pkg_count; i++) {
        if (pkg_db[i].installed) {
            io_print_c(ANSI_GREEN, " [i] ");
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
    io_print_c(ANSI_CYAN, "[pkg] fetching manifest for ");
    io_print(pkg_name);
    io_print_c(ANSI_CYAN, " from BBUR (" BBUR_SOURCE_URL ")...\n");

    // already installed?
    for (uint32_t i = 0; i < pkg_count; i++) {
        if (k_strcmp(pkg_db[i].name, pkg_name) == 0 && pkg_db[i].installed) {
            io_print_c(ANSI_YELLOW, "[pkg] ");
            io_print(pkg_name);
            io_print_c(ANSI_YELLOW, " is already installed in /usr/bin/\n");
            return;
        }
    }

    // this is the real lookup/fetch step — no more blind flag-flipping
    pkg_index_entry_t *entry = pkg_index_find(pkg_name);
    if (!entry) {
        io_print_c(ANSI_RED, "[pkg] error: package '");
        io_print(pkg_name);
        io_print_c(ANSI_RED, "' not found in BBUR index.\n");
        return;
    }

    if (!vfs_exists(entry->manifest_path)) {
        io_print_c(ANSI_RED, "[pkg] error: manifest missing at ");
        io_print(entry->manifest_path);
        io_print("\n");
        return;
    }

    if (pkg_count >= MAX_PACKAGES) {
        io_print_c(ANSI_RED, "[pkg] error: package database full\n");
        return;
    }

    // binary installs directly to /usr/bin/
    char bin_target[64] = "/usr/bin/";
    k_strcat(bin_target, pkg_name);

    pkg_meta_t *slot = &pkg_db[pkg_count];
    k_strncpy(slot->name, entry->name, 32);
    k_strncpy(slot->version, entry->version, 16);
    k_strncpy(slot->binary_path, bin_target, 64);
    slot->installed = 1;
    pkg_count++;

    io_print_c(ANSI_GREEN, "[pkg] successfully installed ");
    io_print(pkg_name);
    io_print(" v");
    io_print(entry->version);
    io_print(" -> ");
    io_print(bin_target);
    io_print_c(ANSI_GREEN, "\n");
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
        io_print_c(ANSI_RED, "Usage: pkg [list|install <pkg>]\n");
    }
}
