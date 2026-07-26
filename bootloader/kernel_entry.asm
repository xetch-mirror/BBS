bits 32
global entry_32bit

entry_32bit:
    ; Load the remaining sectors from disk
    mov eax, 2              
    mov ecx, 64             
    mov edi, 0x10000        
    call read_ata_sectors

    ; Jump to the kernel loaded at physical address 0x10000.
    mov eax, 0x10000
    call eax
    
    cli
    hlt
    jmp $
