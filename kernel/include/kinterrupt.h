#ifndef KINTERRUPT_H
#define KINTERRUPT_H

#include "Clib/Xbool.h"

// CPU register frame saved by interrupt wrapper
typedef struct {
    unsigned int vector;   // Interrupt number / IRQ
    unsigned int err_code; // Error code pushed by CPU (if any)
    unsigned int eip;      // Instruction pointer
    unsigned int cs;       // Code segment
    unsigned int eflags;   // CPU Flags
} interrupt_frame_t;

// Function signature for IRQ handlers
typedef void (*irq_handler_t)(interrupt_frame_t *frame);

// Register an IRQ handler (e.g., irq 0 for timer, irq 1 for kbd)
xbool register_irq(unsigned int irq, irq_handler_t handler);
void unregister_irq(unsigned int irq);

// Low-level CPU interrupt toggles (inline assembly)
static inline void interrupts_enable(void) {
    __asm__ __volatile__("sti");
}

static inline void interrupts_disable(void) {
    __asm__ __volatile__("cli");
}

#endif // KINTERRUPT_H
