#ifndef VFS_H
#define VFS_H

#include <stdint.h>

#define VFS_NAME_MAX 64
#define VFS_MAX_FILES 128

#define VFS_OK       0
#define VFS_ERROR   -1
#define VFS_ENOENT  -2 
#define VFS_ENOMEM  -3 

static inline int vfs_strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

struct vfs_node;

typedef struct {
    struct vfs_node *node;
    uint32_t offset;
    uint32_t flags;
} vfs_file_t;

typedef struct {
    int (*read)(struct vfs_node *node, uint32_t offset, void *buffer, uint32_t size);
    int (*write)(struct vfs_node *node, uint32_t offset, const void *buffer, uint32_t size);
} vfs_ops_t;

typedef struct vfs_node {
    char name[VFS_NAME_MAX];
    uint32_t size;
    uint32_t is_dir;
    uint32_t inode_id;       
    void *priv_data;         
    vfs_ops_t *ops;          
} vfs_node_t;

static vfs_node_t vfs_registry[VFS_MAX_FILES];
static uint32_t vfs_count = 0;

static inline int vfs_register_node(vfs_node_t node) {
    if (vfs_count >= VFS_MAX_FILES) return VFS_ENOMEM;
    vfs_registry[vfs_count++] = node;
    return VFS_OK;
}

static inline int vfs_open(const char *path, vfs_file_t *out_file) {
    for (uint32_t i = 0; i < vfs_count; i++) {
        if (vfs_strcmp(vfs_registry[i].name, path) == 0) {
            out_file->node = &vfs_registry[i];
            out_file->offset = 0;
            out_file->flags = 0;
            return VFS_OK;
        }
    }
    return VFS_ENOENT;
}

static inline int vfs_read(vfs_file_t *file, void *buffer, uint32_t size) {
    if (!file || !file->node || !file->node->ops || !file->node->ops->read) return VFS_ERROR;
    if (file->offset >= file->node->size) return 0; 
    if (file->offset + size > file->node->size) size = file->node->size - file->offset;

    int bytes_read = file->node->ops->read(file->node, file->offset, buffer, size);
    if (bytes_read > 0) file->offset += bytes_read;
    return bytes_read;
}

static inline int vfs_write(vfs_file_t *file, const void *buffer, uint32_t size) {
    if (!file || !file->node || !file->node->ops || !file->node->ops->write) return VFS_ERROR;
    
    int bytes_written = file->node->ops->write(file->node, file->offset, buffer, size);
    if (bytes_written > 0) {
        file->offset += bytes_written;
        if (file->offset > file->node->size) file->node->size = file->offset;
    }
    return bytes_written;
}

#endif
