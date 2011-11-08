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

bits 64
section .text

extern kmain
global boot

boot:
  cli                           ; Clear interrupts

  mov rsp, stack_top            ; Kernel stack for BSP
  mov rbp, 0x0

  mov rsi, gdtr64               ; Load higher half GDT64
  lgdt [rsi]

  mov ax, 0x10                  ; Reload data segments
  mov gs, ax
  mov ds, ax
  mov fs, ax
  mov es, ax

  call kmain                    ; Kernel main function
  jmp $                         ; Endless loop

;; see memory.h
global memory_space_get
memory_space_get:
  mov rax, cr3
  ret

;; see memory.h
global memory_space_switch
memory_space_switch:
  mov rax, cr3
  mov cr3, rdi
  ret

;; see cpu/tss.c
global cpu_tss_load
cpu_tss_load:
  mov rax, gdtr64
  lgdt [rax]

  mov ax, 0x2B
  ltr ax
  ret

global idle
idle:
  sti
  hlt
  jmp idle

section .data

;; Higher half GDT64 pointer
align 8
global gdtr64
gdtr64:
  dw 0x37
  dq 0xFFFFFF7FFFFFD00A

section .bss

;; Initialization Stack
align 4096
global stack_bottom
global stack_top

stack_bottom:
  resb 0x1000                   ; Reserve 4 KB
stack_top:

;; Runtime Stack
align 4096
global stack_runtime_bottom
global stack_runtime_top

stack_runtime_bottom:
  resb 0x1000                   ; Reserve 4 KB
stack_runtime_top:

;; Initial frame set (see kernel/memory/frame.c)
align 4096
global frame_alloc_init

frame_alloc_init:
  resb 0x1000                   ; Reserve 4 KB
