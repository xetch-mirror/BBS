#include "kernel/include/mouse.h"
#include "init/include/sys_io.h"

#define MOUSE_DATA_PORT   0x60
#define MOUSE_STATUS_PORT 0x64
#define MOUSE_CMD_SEND    0xD4

static mouse_state_t g_mouse = {400, 300, 0, 0, 0}; // Default center screen
static uint8_t mouse_cycle = 0;
static int8_t mouse_bytes[3];

static void mouse_wait(uint8_t type) {
    uint32_t timeout = 100000;
    if (type == 0) {
        while (timeout--) {
            if ((inb(MOUSE_STATUS_PORT) & 1) == 1) return; // Data ready to read
        }
    } else {
        while (timeout--) {
            if ((inb(MOUSE_STATUS_PORT) & 2) == 0) return; // Ready to write
        }
    }
}

static void mouse_write(uint8_t write_val) {
    mouse_wait(1);
    outb(MOUSE_STATUS_PORT, MOUSE_CMD_SEND);
    mouse_wait(1);
    outb(MOUSE_DATA_PORT, write_val);
}

void mouse_init(void) {
    uint8_t status;

    // Enable auxiliary PS/2 mouse device
    mouse_wait(1);
    outb(MOUSE_STATUS_PORT, 0xA8);

    // Get Compaq status byte & enable interrupt (Bit 1)
    mouse_wait(1);
    outb(MOUSE_STATUS_PORT, 0x20);
    mouse_wait(0);
    status = (inb(MOUSE_DATA_PORT) | 2); // Enable IRQ12

    mouse_wait(1);
    outb(MOUSE_STATUS_PORT, 0x60);
    mouse_wait(1);
    outb(MOUSE_DATA_PORT, status);

    // Enable packet streaming mode on mouse
    mouse_write(0xF4);
    mouse_wait(0);
    inb(MOUSE_DATA_PORT); // Read ACK byte

    io_print("[k] PS/2 Mouse initialized.\n");
}

// Process 3-byte hardware packet
void mouse_handle_interrupt(void) {
    uint8_t status = inb(MOUSE_STATUS_PORT);
    if (!(status & 0x01)) return; // No data available

    uint8_t data = inb(MOUSE_DATA_PORT);

    switch (mouse_cycle) {
        case 0:
            if ((data & 0x08) == 0) break; // Alignment check
            mouse_bytes[0] = data;
            mouse_cycle++;
            break;
        case 1:
            mouse_bytes[1] = data;
            mouse_cycle++;
            break;
        case 2:
            mouse_bytes[2] = data;
            mouse_cycle = 0;

            // Extract buttons and movement deltas
            g_mouse.left_button   = (mouse_bytes[0] & 0x01);
            g_mouse.right_button  = (mouse_bytes[0] & 0x02);
            g_mouse.middle_button = (mouse_bytes[0] & 0x04);

            int8_t dx = mouse_bytes[1];
            int8_t dy = mouse_bytes[2];

            g_mouse.x += dx;
            g_mouse.y -= dy; // Invert Y delta for top-down framebuffer rendering

            // Screen boundary clamping (assuming 800x600 resolution)
            if (g_mouse.x < 0) g_mouse.x = 0;
            if (g_mouse.x > 799) g_mouse.x = 799;
            if (g_mouse.y < 0) g_mouse.y = 0;
            if (g_mouse.y > 599) g_mouse.y = 599;
            break;
    }
}

// Render the iconic RHEL 8x8 Solid Black Square Cursor
void mouse_draw_black_square(uint32_t *framebuffer, uint32_t screen_width) {
    int cursor_size = 8; // 8x8 block pixels

    for (int dy = 0; dy < cursor_size; dy++) {
        for (int dx = 0; dx < cursor_size; dx++) {
            int px = g_mouse.x + dx;
            int py = g_mouse.y + dy;

            // Render pure black pixel (0x00000000)
            framebuffer[py * screen_width + px] = 0x00000000;
        }
    }
}
