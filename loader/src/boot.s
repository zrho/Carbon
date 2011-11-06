; Carbon Operating System
; Copyright (C) 2011 Lukas Heidemann
;
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>.

bits 32
section .text
extern init
extern stack_end
extern boot_pml4
extern gdtr64

; Boot entry point
;
; Parameters:
; 	eax Multiboot magic number
; 	ebx Multiboot info pointer
global boot
boot:
  cli                           ; Clear interrupts
  mov esp, stack_end            ; Load stack

  xor ecx, ecx                  ; Clear registers (except EAX and EBX)
  xor edx, edx
  xor edi, edi
  xor esi, esi
  xor ebp, ebp

  push ebx                      ; Push multiboot magic
  push eax                      ; Push multiboot info ptr
  call init                     ; Sets up the system's memory and loads the kernel

  call boot_a20                 ; Enable A20 gate
  call boot_pae                 ; Enable PAE
  call boot_long_mode           ; Enable long mode MSR
  call boot_paging              ; Enable paging (will be in compatibility mode afterwards
  call boot_gdt64               ; Load 64 bit GDT
  
  jmp 0x08:boot_trampoline       ; Far jump into 64 trampoline code

; 64 bit boot trampoline code.
;
; Jump target injected in main().
global boot_trampoline_callback

boot_trampoline:
  db 0x48                       ; mov rax, 
  db 0xb8
boot_trampoline_callback:
  dd 0x0
  dd 0x0
  db 0xff                       ; jmp rax
  db 0xe0

; Sets up long mode
;
; Affects Registers:
; 	eax
; 	ecx
boot_long_mode:
  mov ecx, 0xC0000080
  rdmsr
  or eax, 1 << 8
  wrmsr
  ret

; Sets up A20 gate
;
; Affects Registers:
; 	al
boot_a20:
  in al, 0x92
  or al, 2
  out 0x92, al
  ret

; Enables PAE
;
; Affects Registers:
; 	eax
boot_pae:
  mov eax, cr4
  or eax, 1 << 5
  mov cr4, eax
  ret

; Sets the boot PML4 and enables paging
;
; Affects Registers:
; 	eax
boot_paging:
  mov eax, boot_pml4            ; Load PML4
  mov cr3, eax

  mov eax, cr0                  ; Enable paging
  or eax, 1 << 31
  mov cr0, eax

  ret

; Loads the GDT64 using the lower half GDT pointer
;
; Affects Registers:
; 	eax
boot_gdt64:
  mov eax, gdtr64
  lgdt [eax]
  ret
