#include "panic_config.h"

// custom OS global pointer to the active graphics VRAM memory address
extern uint32_t* system_frame_buffer; 

// Your built-in 8x8 font bitmap (each char is 8 bytes, 1 byte per horizontal row)
extern const uint8_t basic_font_8x8[128][8];

// Draw a solid color block over the active graphics screen
void draw_solid_box(int start_x, int start_y, int width, int height, uint32_t color) {
    for (int y = start_y; y < (start_y + height); y++) {
        for (int x = start_x; x < (start_x + width); x++) {
            system_frame_buffer[y * SCREEN_WIDTH + x] = color;
        }
    }
}

// Draw a single 8x8 bitmap character directly to the frame buffer
void draw_char(char c, int start_x, int start_y, uint32_t color) {
    for (int row = 0; row < 8; row++) {
        uint8_t row_bits = basic_font_8x8[(uint8_t)c][row];
        for (int col = 0; col < 8; col++) {
            if (row_bits & (0x80 >> col)) {
                system_frame_buffer[(start_y + row) * SCREEN_WIDTH + (start_x + col)] = color;
            }
        }
    }
}

// Loop through a string and draw it pixel-by-pixel
void draw_string(const char* str, int x, int y, uint32_t color) {
    int current_x = x;
    while (*str) {
        draw_char(*str, current_x, y, color);
        current_x += 8; // Advance 8 horizontal pixels per char
        str++;
    }
}

// The final system execution stop point
void kernel_panic(const char* primary_reason, const char* secondary_reason) {
    // 1. Draw the black window overlay over the frozen desktop graphics
    draw_solid_box(BOX_X, BOX_Y, BOX_WIDTH, BOX_HEIGHT, COLOR_BLACK);
    
    // 2. Render a Red border frame
    draw_solid_box(BOX_X, BOX_Y, BOX_WIDTH, 2, COLOR_RED);                           // Top
    draw_solid_box(BOX_X, BOX_Y + BOX_HEIGHT - 2, BOX_WIDTH, 2, COLOR_RED);          // Bottom
    draw_solid_box(BOX_X, BOX_Y, 2, BOX_HEIGHT, COLOR_RED);                           // Left
    draw_solid_box(BOX_X + BOX_WIDTH - 2, BOX_Y, 2, BOX_HEIGHT, COLOR_RED);          // Right

    // 3. Render the static header elements
    draw_string("KERNEL PANIC!", BOX_X + 20, BOX_Y + 20, COLOR_RED);
    draw_string("=======================================", BOX_X + 20, BOX_Y + 35, COLOR_WHITE);
    
    // 4. Render the dynamic system error path
    draw_string("Reason:", BOX_X + 20, BOX_Y + 55, COLOR_WHITE);
    draw_string(primary_reason, BOX_X + 30, BOX_Y + 75, COLOR_WHITE);
    
    if (secondary_reason != NULL) {
        draw_string(secondary_reason, BOX_X + 20, BOX_Y + 110, COLOR_WHITE);
    }
    
    draw_string("System execution halted permanently.", BOX_X + 20, BOX_Y + 140, COLOR_RED);

    // 5. Tell the CPU to go into a deep, non-executing state
    asm volatile (
        "cli\n\t"    // Clear interrupts so the timer can't wake it
        "1:\n\t"
        "hlt\n\t"    // Halt Execution 
        "jmp 1b"     // loops
    );
}

