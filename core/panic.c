#include "panic_config.h"
#include "Clib/Xlibary/Xstring.h"
#include <stdint.h>

extern void serial_write(const char *str);

static void serial_write_hex(uint32_t val) {
    char buf[32];
    print(val, buf, 16);
    serial_write(buf);
}

void kernel_panic(int vfs_exists, int ext4_exists, int init_exists, uint32_t fault_address) {
    serial_write("\n");
    
    serial_write("kernel offset: 0x");
    serial_write_hex(fault_address);
    serial_write("\n");
    
    serial_write("kernel panic - not syncing: ");
    
    if (!vfs_exists) {
        serial_write("VFS not syncing\n");
    } else if (!ext4_exists) {
        serial_write("rootfs not syncing\n");
    } else if (!init_exists) {
        serial_write("Attempted to kill init! exitcode=0x00000007 \n");
    } else {
        serial_write("fatal exception in interrupt handler\n");
    }
    
    serial_write("\n");
    serial_write("you can safely force shutdown now.\n");

    asm volatile (
        "cli\n\t"
        "1:\n\t"
        "hlt\n\t"
        "jmp 1b"
    );
}
