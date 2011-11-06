OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)
ENTRY(boot)
SECTIONS
{
    .text 0x100000 :
    {
        code = .; _code = .; __code = .;
        *(.multiboot)
        *(.text)
    }

    . = ALIGN(4096);

    .data :
    {
        data = .; _data = .; __data = .;
        *(.data)
        *(.rodata)
        . = ALIGN(4096);
    }

    .bss :
    {
        bss = .; _bss = .; __bss = .;
        *(.bss)
        . = ALIGN(4096);
    }

    end = .; _end = .; __end = .;
}
