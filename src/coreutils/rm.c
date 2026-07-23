#include <stdint.h>
#include "kernel/include/vfs.h"
#include "kernel/include/ksubsys.h"

void klog(const ksubsys_t subsys, const char* msg);
void ata_read_sector(uint32_t lba, uint8_t *buf);
void ata_write_sector(uint32_t lba, const uint8_t *buf);

#define SECTOR_SIZE 512
#define MAX_DIR_ENTRIES (SECTOR_SIZE / sizeof(vfs_disk_entry_t))
#define BFS_QUEUE_CAPACITY 64

typedef struct {
    char name[VFS_NAME_MAX];
    uint32_t lba;
    uint32_t size;
    uint32_t is_dir;
    uint32_t is_used;
    uint32_t read_only;
} __attribute__((packed)) vfs_disk_entry_t;

typedef struct {
    uint32_t lba;
    uint32_t parent_lba;
    int parent_slot;
} bfs_node_t;

int sys_rm(uint32_t target_lba, uint32_t parent_lba, int parent_slot, int recursive, int force, int verbose) {
    bfs_node_t queue[BFS_QUEUE_CAPACITY];
    int head = 0, tail = 0;
    uint8_t sector_buf[SECTOR_SIZE];

    queue[tail++] = (bfs_node_t){
        .lba = target_lba,
        .parent_lba = parent_lba,
        .parent_slot = parent_slot
    };

    while (head < tail) {
        bfs_node_t current = queue[head++];
        ata_read_sector(current.lba, sector_buf);
        vfs_disk_entry_t *entries = (vfs_disk_entry_t *)sector_buf;

        for (int i = 0; i < (int)MAX_DIR_ENTRIES; i++) {
            if (!entries[i].is_used) continue;

            if (entries[i].read_only && !force) {
                if (verbose) {
                    klog(LOG_DISK, "rm error: file is read-only (force flag -f required)");
                }
                return VFS_ERROR;
            }

            if (entries[i].is_dir) {
                if (!recursive) {
                    if (verbose) {
                        klog(LOG_DISK, "rm error: target is a directory (recursive flag -r required)");
                    }
                    return VFS_ERROR;
                }

                if (tail < BFS_QUEUE_CAPACITY) {
                    queue[tail++] = (bfs_node_t){
                        .lba = entries[i].lba,
                        .parent_lba = current.lba,
                        .parent_slot = i
                    };
                }
            } else {
                entries[i].is_used = 0;
                if (verbose) {
                    klog(LOG_DISK, "rm: child file removed");
                }
            }
        }

        ata_write_sector(current.lba, sector_buf);
    }

    for (int i = tail - 1; i >= 0; i--) {
        uint8_t zero_buf[SECTOR_SIZE] = {0};
        ata_write_sector(queue[i].lba, zero_buf);

        if (queue[i].parent_lba != 0) {
            ata_read_sector(queue[i].parent_lba, sector_buf);
            vfs_disk_entry_t *p_entries = (vfs_disk_entry_t *)sector_buf;
            p_entries[queue[i].parent_slot].is_used = 0;
            ata_write_sector(queue[i].parent_lba, sector_buf);
        }
    }

    if (verbose) {
        klog(LOG_DISK, "rm: target path recursively removed");
    }

    return VFS_OK;
}
