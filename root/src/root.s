bits 64
section .text

global debug
debug:
    mov rax, 40
    xchg rbx, rdi
    int 0x80
    ret

global debug_hex
debug_hex:
    mov rax, 41
    xchg rbx, rdi
    int 0x80
    ret

global _main
extern main
_main:
	call main
	jmp $
