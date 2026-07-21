#include <stdint.h>
#include "include/vfs.h"
#include "include/Kinterrupt.h"
#include "include/Kio_ports.h"
#include "include/Ksubsys.h"
#include "include/Ktime.h"

#define VGA_ADDR 0xB8000
#define VGA_COLS 80
#define VGA_ROWS 25
#define VGA_ATTR 0x0F

#define ATA_DATA 0x1F0
#define ATA_ERR  0x1F1
#define ATA_SEC  0x1F2
#define ATA_LBA1 0x1F3
#define ATA_LBA2 0x1F4
#define ATA_LBA3 0x1F5
#define ATA_DRV  0x1F6
#define ATA_CMD  0x1F7
#define SECTOR_SZ 512

static volatile uint16_t* const VGA_BUFFER = (uint16_t*)VGA_ADDR;
static int vga_col = 0;
static int vga_row = 0;

static void vga_scroll(void) {
    for (int i = 0; i < (VGA_ROWS - 1) * VGA_COLS; i++) {
        VGA_BUFFER[i] = VGA_BUFFER[i + VGA_COLS];
    }
    for (int i = (VGA_ROWS - 1) * VGA_COLS; i < VGA_ROWS * VGA_COLS; i++) {
        VGA_BUFFER[i] = (uint16_t)' ' | (VGA_ATTR << 8);
    }
    vga_row = VGA_ROWS - 1;
}

void io_print(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            vga_col = 0;
            vga_row++;
            if (vga_row >= VGA_ROWS) vga_scroll();
            continue;
        }
        VGA_BUFFER[vga_row * VGA_COLS + vga_col] = (uint16_t)str[i] | (VGA_ATTR << 8);
        vga_col++;
        if (vga_col >= VGA_COLS) {
            vga_col = 0;
            vga_row++;
            if (vga_row >= VGA_ROWS) vga_scroll();
        }
    }
}

static void ata_wait(void) {
    while ((inb(ATA_CMD) & 0x80) != 0);
    while ((inb(ATA_CMD) & 0x08) == 0);
}

void ata_read_sector(uint32_t lba, uint8_t *buf) {
    outb(ATA_DRV, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_ERR, 0x00);
    outb(ATA_SEC, 1);
    outb(ATA_LBA1, (uint8_t)(lba & 0xFF));
    outb(ATA_LBA2, (uint8_t)((lba >> 8) & 0xFF));
    outb(ATA_LBA3, (uint8_t)((lba >> 16) & 0xFF));
    outb(ATA_CMD, 0x20);
    ata_wait();
    uint16_t *b16 = (uint16_t*)buf;
    for (int i = 0; i < SECTOR_SZ / 2; i++) b16[i] = inw(ATA_DATA);
}

void ata_write_sector(uint32_t lba, const uint8_t *buf) {
    outb(ATA_DRV, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_ERR, 0x00);
    outb(ATA_SEC, 1);
    outb(ATA_LBA1, (uint8_t)(lba & 0xFF));
    outb(ATA_LBA2, (uint8_t)((lba >> 8) & 0xFF));
    outb(ATA_LBA3, (uint8_t)((lba >> 16) & 0xFF));
    outb(ATA_CMD, 0x30);
    ata_wait();
    const uint16_t *b16 = (const uint16_t*)buf;
    for (int i = 0; i < SECTOR_SZ / 2; i++) outw(ATA_DATA, b16[i]);
}

static int sda1_vfs_read(vfs_node_t *node, uint32_t offset, void *buf, uint32_t size) {
    (void)node;
    uint32_t lba = offset / SECTOR_SZ;
    ata_read_sector(lba, (uint8_t*)buf);
    return size;
}

static int sda1_vfs_write(vfs_node_t *node, uint32_t offset, const void *buf, uint32_t size) {
    (void)node;
    uint32_t lba = offset / SECTOR_SZ;
    ata_write_sector(lba, (const uint8_t*)buf);
    return size;
}

static vfs_ops_t sda1_ops = {
    .read = sda1_vfs_read,
    .write = sda1_vfs_write
};

void init_vfs_devices(void) {
    vfs_node_t dev_sda1 = {
        .name = "/disk/sda1",
        .size = 0,
        .is_dir = 0,
        .inode_id = 2,
        .ops = &sda1_ops
    };
    vfs_register_node(dev_sda1);
}

static uint32_t g_ticks = 0;
ktime_t ktime_get(void) {
    g_ticks++;
    ktime_t t;
    t.seconds = g_ticks / 1000;
    t.microseconds = (g_ticks % 1000) * 1000;
    return t;
}

void klog(const ksubsys_t subsys, const char* msg) {
    char time_str[15];
    ktime_t now = ktime_get();
    ktime_format(now, time_str);

    io_print(time_str);
    io_print("[");
    io_print(subsys.tag);
    io_print("] ");
    io_print(msg);
    io_print("\n");
}

void init_main(void);

void kmain(void) {
    for (int i = 0; i < VGA_COLS * VGA_ROWS; i++) {
        VGA_BUFFER[i] = (uint16_t)' ' | (VGA_ATTR << 8);
    }

    klog(LOG_CLOCK, "timer subsystem online");
    klog(LOG_DISK,  "mounting vfs block node /disk/sda1...");

    init_vfs_devices();

    klog(LOG_DISK,  "vfs storage driver mapped to ide primary master");
    klog(LOG_INPUT, "handing execution over to disk supervisor...");

    init_main();
}
