OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)
ENTRY(BBS_kernel)

SECTIONS
{
    . = 0x10000;

    .text : ALIGN(16)
    {
        *(.text.entry)
        *(.text*)
    }

    .rodata : ALIGN(16)
    {
        *(.rodata*)
    }

    .data : ALIGN(16)
    {
        *(.data*)
    }

    .bss : ALIGN(16)
    {
        *(COMMON)
        *(.bss*)
    }
}