bits 32
global read_ata_sectors

; Parameters: EAX = LBA Sector, ECX = Sector Count, EDI = Target Memory Offset
read_ata_sectors:
    pushad
    
    mov edx, 0x1F6          ; Port: Drive/Head Select
    shr eax, 24
    or al, 0xE0             ; Select master drive
    out dx, al

    mov edx, 0x1F2          ; Port: Sector Count
    mov al, cl
    out dx, al

    ; Send the 24-bit LBA block address
    mov edx, 0x1F3          ; LBA Low
    mov eax, [esp + 28]     ; Get original EAX from stack
    out dx, al

    mov edx, 0x1F4          ; LBA Mid
    mov al, ah
    out dx, al

    mov edx, 0x1F5          ; LBA High
    shr eax, 16
    out dx, al

    mov edx, 0x1F7          ; Command Port
    mov al, 0x20            ; 0x20 = Read Sectors with retry
    out dx, al

.next_sector:
    push ecx                ; Save total sector count

.wait_ready:
    in al, dx
    test al, 0x08           ; Check DRQ bit (Data Request Ready)
    jz .wait_ready

    mov ecx, 256            ; 256 words = 512 bytes per sector
    mov edx, 0x1F0          ; Data port
    rep insw                ; Stream data into [EDI]

    pop ecx
    loop .next_sector

    popad
    ret
