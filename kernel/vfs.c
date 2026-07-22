#include "kernel/include/vfs.h"
#include "include/ksubsys.h"
#include "init/include/sys_io.h"
#include "include/sys_string.h"

extern void ksubsys_log(const ksubsys_t *subsys, const char *msg);
DEFINE_SUBSYS(LOG_VFS, "vfs");

static vfs_node_t vfs_nodes[VFS_MAX_NODES];
static uint32_t node_count = 0;

void vfs_init(void) {
    node_count = 0;

    // Root FHS directories
    vfs_mkdir("/bin");
    vfs_mkdir("/dev");
    vfs_mkdir("/etc");
    vfs_mkdir("/home");
    vfs_mkdir("/lib");
    vfs_mkdir("/mnt");
    vfs_mkdir("/opt");
    vfs_mkdir("/proc");
    vfs_mkdir("/root");
    vfs_mkdir("/sbin");

    // /usr hierarchy
    vfs_mkdir("/usr");
    vfs_mkdir("/usr/bin");
    vfs_mkdir("/usr/include");
    vfs_mkdir("/usr/lib");
    vfs_mkdir("/usr/share");
    vfs_mkdir("/usr/share/man");

    // /var hierarchy
    vfs_mkdir("/var");
    vfs_mkdir("/var/lib");
    vfs_mkdir("/var/lock");
    vfs_mkdir("/var/log");
    vfs_mkdir("/var/opt");
    vfs_mkdir("/var/spool");
    vfs_mkdir("/var/tmp");

    ksubsys_log(&LOG_VFS, "FHS directory tree created");
}

int vfs_register_node(vfs_node_t node) {
    if (node_count >= VFS_MAX_NODES) return -1;
    if (node.inode_id == 0) node.inode_id = node_count + 1;
    vfs_nodes[node_count++] = node;
    return 0;
}

vfs_node_t *vfs_lookup(const char *path) {
    if (!path) return 0;
    for (uint32_t i = 0; i < node_count; i++) {
        if (k_strcmp(vfs_nodes[i].name, path) == 0) return &vfs_nodes[i];
    }
    return 0;
}

int vfs_exists(const char *path) {
    return (vfs_lookup(path) != 0);
}

int vfs_mkdir(const char *path) {
    if (vfs_exists(path)) return 0;
    vfs_node_t node;
    k_strncpy(node.name, path, VFS_MAX_PATH);
    node.size = 0;
    node.flags = VFS_DIRECTORY;
    node.inode_id = node_count + 1;
    node.ops = 0;
    return vfs_register_node(node);
}

void vfs_list_dir(const char *path) {
    (void)path;
    io_print("  TYPE     INODE    NAME\n");
    io_print("  -----------------------------------\n");
    for (uint32_t i = 0; i < node_count; i++) {
        if (vfs_nodes[i].flags & VFS_DIRECTORY)      io_print("  [DIR]   ");
        else if (vfs_nodes[i].flags & VFS_BLOCKDEV) io_print("  [DEV]   ");
        else                                        io_print("  [FILE]  ");

        io_print("  ");
        io_print(vfs_nodes[i].name);
        io_print("\n");
    }
}
