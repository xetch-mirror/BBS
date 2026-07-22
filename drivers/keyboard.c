#include "Clib/io.h"
#include "Clib/iso646.h"

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

#define KB_DATA_PORT   0x60
#define KB_STATUS_PORT 0x64
#define KB_STATUS_MASK 0x01

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_LIMIT (VGA_WIDTH * VGA_HEIGHT)

uint8_t bbs_keyboard_get_scancode(void) {
    while (not (_inp(KB_STATUS_PORT) and KB_STATUS_MASK)) {
        __asm__ __volatile__("pause"); 
    }
    return _inp(KB_DATA_PORT);
}

const char kbd_us[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',  
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',      
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,         
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};

char bbs_get_char(void) {
    uint8_t scancode = bbs_keyboard_get_scancode();

    // 1. ignore key release events (when bit 7 is set)
    if (scancode & 0x80) {
        return 0; 
    }

    // 2. prevent array index out of bounds
    if (scancode >= 128) {
        return 0;
    }

    return kbd_us[scancode];
}


void bbs_simple_shell(void) {
    volatile char *vga = (volatile char*)0xB8000;
    int cursor = 0;

    // clear screen first so it's clean
    for (int i = 0; i < VGA_LIMIT * 2; i += 2) {
        vga[i] = ' ';
        vga[i + 1] = 0x07;
    }

    while (1) {
        char c = bbs_get_char();
        if (c != 0) {
            // check for backspace
            if (c == '\b' and cursor > 0) {
                cursor--;
                vga[cursor * 2] = ' ';
                vga[cursor * 2 + 1] = 0x07;
            } 
            // check for Enter/Newline
            else if (c == '\n') {
                // move cursor to the start of the next line
                cursor = ((cursor / VGA_WIDTH) + 1) * VGA_WIDTH;
            } 
            // regular printable character
            else if (c != '\b' and cursor < VGA_LIMIT) {
                vga[cursor * 2] = c;
                vga[cursor * 2 + 1] = 0x0F; // Bright white text
                cursor++;
            }
            
            // screen roll-over safety
            if (cursor >= VGA_LIMIT) {
                cursor = 0; // wrap back to the top for now
            }
        }
    }
}
