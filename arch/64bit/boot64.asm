bits 32
global entry_32bit
extern read_ata_sectors
extern BBS_kernel

section .text
entry_32bit:
    mov eax, 2
    mov ecx, 64
    mov edi, 0x10000
    call read_ata_sectors

    mov edi, 0x1000
    mov cr3, edi
    
    xor eax, eax
    mov ecx, 4096
    rep stosd

    mov dword [0x1000], 0x2003
    mov dword [0x2000], 0x3003
    mov dword [0x3000], 0x0083

    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    lgdt [gdt64_pointer]
    jmp 0x08:entry_64bit

bits 64
entry_64bit:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    call BBS_kernel

    cli
    hlt
    jmp $

align 8
gdt64:
    dq 0
    dq 0x00AF9A0000000000
    dq 0x00CF920000000000
gdt64_pointer:
    dw $ - gdt64 - 1
    dq gdt64
