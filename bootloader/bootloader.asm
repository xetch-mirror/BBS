[org 0x7c00]
bits 16

start:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    cli                    ; Disable interrupts for GDT switch
    lgdt [gdt_descriptor]  ; Load GDT
    
    ; Switch to Protected Mode
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    
    jmp 0x08:init_32bit    ; Far jump to clear CPU pipeline

%include "gdt.asm"

bits 32
init_32bit:
    mov ax, 0x10            ; Update data segments
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000        ; Set up stack pointer

    jmp entry_32bit

%include "disk.asm"
%include "kernel_entry.asm"

times 510 - ($ - $$) db 0
dw 0xaa55
