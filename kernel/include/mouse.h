#ifndef MOUSE_H
#define MOUSE_H

#include "sys_types.h"

typedef struct {
    int x;
    int y;
    uint8_t left_button;
    uint8_t right_button;
    uint8_t middle_button;
} mouse_state_t;

void mouse_init(void);
void mouse_handle_interrupt(void);
void mouse_draw_black_square(uint32_t *framebuffer, uint32_t pitch);

#endif
