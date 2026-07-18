; gdt.asm
gdt_start:
    dd 0x0                ; Null descriptor
    dd 0x0

gdt_code:                 ; Code segment descriptor
    dw 0xffff             ; Limit
    dw 0x0                ; Base
    db 0x0                ; Base
    db 10011010b          ; Access byte
    db 11001111b          ; Flags
    db 0x0                ; Base

gdt_data:                 ; Data segment descriptor
    dw 0xffff
    dw 0x0
    db 0x0
    db 10010010b          ; Access byte
    db 11001111b          ; Flags
    db 0x0

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start
