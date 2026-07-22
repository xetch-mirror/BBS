#include "kernel/include/vfs.h"
#include "init/include/sys_io.h"
#include "include/sys_string.h"

static vfs_node_t vfs_nodes[VFS_MAX_NODES];
static uint32_t node_count = 0;

void vfs_init(void) {
    node_count = 0;

    // standard flat directory hierarchy
    vfs_mkdir("/usr");
    vfs_mkdir("/usr/bin");
    vfs_mkdir("/disk");
    vfs_mkdir("/disk/sda1");

    io_print("[vfs] Virtual File System core online\n");
}

int vfs_register_node(vfs_node_t node) {
    if (node_count >= VFS_MAX_NODES) {
        io_print("[vfs_err] maximum node table capacity reached!\n");
        return -1;
    }

    // assign node index if not explicitly bound
    if (node.inode_id == 0) {
        node.inode_id = node_count + 1;
    }

    vfs_nodes[node_count++] = node;
    return 0;
}

vfs_node_t *vfs_lookup(const char *path) {
    if (!path) return 0;

    for (uint32_t i = 0; i < node_count; i++) {
        if (k_strcmp(vfs_nodes[i].name, path) == 0) {
            return &vfs_nodes[i];
        }
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

int vfs_create_file(const char *path, const uint8_t *data, uint32_t size) {
    (void)data; // Can be hooked into extf4_write_file if backed by disk
    vfs_node_t node;
    k_strncpy(node.name, path, VFS_MAX_PATH);
    node.size = size;
    node.flags = VFS_FILE;
    node.inode_id = node_count + 1;
    node.ops = 0;

    return vfs_register_node(node);
}

int vfs_read(vfs_node_t *node, uint32_t offset, void *buf, uint32_t size) {
    if (!node) return -1;
    if (node->ops && node->ops->read) {
        return node->ops->read(node, offset, buf, size);
    }
    return -1;
}

int vfs_write(vfs_node_t *node, uint32_t offset, const void *buf, uint32_t size) {
    if (!node) return -1;
    if (node->ops && node->ops->write) {
        return node->ops->write(node, offset, buf, size);
    }
    return -1;
}

void vfs_list_dir(const char *path) {
    (void)path;
    io_print("  TYPE     INODE    SIZE        NAME\n");
    io_print("  -------------------------------------------\n");
    for (uint32_t i = 0; i < node_count; i++) {
        if (vfs_nodes[i].flags & VFS_DIRECTORY) {
            io_print("  [DIR]   ");
        } else if (vfs_nodes[i].flags & VFS_BLOCKDEV) {
            io_print("  [DEV]   ");
        } else {
            io_print("  [FILE]  ");
        }

        io_print("  ");
        io_print(vfs_nodes[i].name);
        io_print("\n");
    }
}
