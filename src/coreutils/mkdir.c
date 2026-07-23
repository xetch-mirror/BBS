#include <stdint.h>
#include "kernel/include/vfs.h"
#include "kernel/include/ksubsys.h"

void klog(const ksubsys_t subsys, const char* msg);
void ata_read_sector(uint32_t lba, uint8_t *buf);
void ata_write_sector(uint32_t lba, const uint8_t *buf);

#define SECTOR_SIZE 512
#define MAX_DIR_ENTRIES (SECTOR_SIZE / sizeof(vfs_disk_entry_t))

typedef struct {
    char name[VFS_NAME_MAX];
    uint32_t lba;
    uint32_t size;
    uint32_t is_dir;
    uint32_t is_used;
    uint32_t read_only;
} __attribute__((packed)) vfs_disk_entry_t;

static void k_strcpy(char *dest, const char *src, uint32_t max_len) {
    uint32_t i = 0;
    while (src[i] != '\0' && i < (max_len - 1)) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

int sys_mkdir(uint32_t parent_lba, const char *name, int verbose) {
    uint8_t sector_buf[SECTOR_SIZE];
    vfs_disk_entry_t *entries = (vfs_disk_entry_t *)sector_buf;

    ata_read_sector(parent_lba, sector_buf);

    int free_slot = -1;

    for (int i = 0; i < (int)MAX_DIR_ENTRIES; i++) {
        if (entries[i].is_used && vfs_strcmp(entries[i].name, name) == 0) {
            if (verbose) {
                klog(LOG_DISK, "mkdir: directory already exists, skipping");
            }
            return VFS_OK;
        }

        if (!entries[i].is_used && free_slot == -1) {
            free_slot = i;
        }
    }

    if (free_slot == -1) {
        if (verbose) klog(LOG_DISK, "mkdir error: parent directory full");
        return VFS_ERROR;
    }

    static uint32_t next_free_lba = 200; 
    uint32_t new_dir_lba = next_free_lba++;

    uint8_t new_dir_buf[SECTOR_SIZE] = {0};
    ata_write_sector(new_dir_lba, new_dir_buf);

    entries[free_slot].lba = new_dir_lba;
    entries[free_slot].is_dir = 1;
    entries[free_slot].is_used = 1;
    entries[free_slot].read_only = 0;
    entries[free_slot].size = 0;
    k_strcpy(entries[free_slot].name, name, VFS_NAME_MAX);

    ata_write_sector(parent_lba, sector_buf);

    vfs_node_t dir_node;
    dir_node.size = 0;
    dir_node.is_dir = 1;
    dir_node.inode_id = new_dir_lba;
    dir_node.ops = 0;
    k_strcpy(dir_node.name, name, VFS_NAME_MAX);
    vfs_register_node(dir_node);

    if (verbose) {
        klog(LOG_DISK, "mkdir: directory created successfully");
    }

    return VFS_OK;
}
