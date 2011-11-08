OUTPUT_FORMAT(elf64-x86-64)
KERNEL_VMA = 0x100000;
ENTRY(_start)
SECTIONS
{
    . = KERNEL_VMA;

    .text : AT(ADDR(.text) - KERNEL_VMA)
    {
        code = .;
        *(.text)
        *(.rodata*)
        . = ALIGN(4096);
    }

   .data : AT(ADDR(.data) - KERNEL_VMA)
   {
        data = .;
        *(.data)
        . = ALIGN(4096);
   }

   .ehframe : AT(ADDR(.ehframe) - KERNEL_VMA)
   {
       ehframe = .;
       *(.ehframe)
        . = ALIGN(4096);
   }

   .bss : AT(ADDR(.bss) - KERNEL_VMA)
   {
       __bss_start = .;
       *(.bss)
       
        *(COMMON)
       . = ALIGN(4096);
       
       _elf64_aux_page = .;
       . += 4096;
   }

   _end = .;

   /DISCARD/ :
   {
        *(.comment)
   }
}
