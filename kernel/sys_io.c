#include "init/include/sys_io.h"

void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void io_wait(void) {
    // Port 0x80 is used for POST checkpoints; writing to it forces a tiny CPU delay
    outb(0x80, 0x00);
}
