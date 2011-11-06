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

;; Imports
extern cpu_int_handle

;; 64 Bit code
section .text

global cpu_int_enable
cpu_int_enable:
  sti
  ret

global cpu_int_disable
cpu_int_disable:
  cli
  ret

global cpu_int_lidt
cpu_int_lidt:
  mov rax, cpu_int_idtr
  lidt [rax]
  ret

cpu_int_handler_common:
  mov r15, cr2

  push rax                      ; Push registers
  push rbx
  push rcx
  push rdx
  push rbp
  push rdi
  push rsi
  push r8
  push r9
  push r10
  push r11
  push r12
  push r13
  push r14
  push r15

  xor rax, rax
  mov ax, ds                    ; Back up data segment
  push rax

  mov ax, 0x10                  ; Load kernel data segment
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  mov rdi, rsp                  ; Stack pointer as parameter
  call cpu_int_handle

  pop rax                       ; Restore data segment
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  pop r15                       ; Restore registers
  pop r14
  pop r13
  pop r12
  pop r11
  pop r10
  pop r9
  pop r8
  pop rsi
  pop rdi
  pop rbp
  pop rdx
  pop rcx
  pop rbx
  pop rax

  add rsp, 16                   ; Clean up error code and interrupt vector
  iretq

; Macro for non-error interrupts
; Pushes a fake error code (zero) to create a stack of equal length
%macro INT_NOERRCODE 1
    cpu_int_stub%1:
        cli
        push qword 0
        push qword %1
        jmp cpu_int_handler_common
%endmacro

; Macro for error-code interrupts
%macro INT_ERRCODE 1
    cpu_int_stub%1:
        cli
        push qword %1
        jmp cpu_int_handler_common
%endmacro

; Exception interrupts
INT_NOERRCODE 0
INT_NOERRCODE 1
INT_NOERRCODE 2
INT_NOERRCODE 3
INT_NOERRCODE 4
INT_NOERRCODE 5
INT_NOERRCODE 6
INT_NOERRCODE 7
INT_ERRCODE   8
INT_NOERRCODE 9
INT_ERRCODE   10
INT_ERRCODE   11
INT_ERRCODE   12
INT_ERRCODE   13
INT_ERRCODE   14
INT_NOERRCODE 15
INT_NOERRCODE 16
INT_NOERRCODE 17
INT_NOERRCODE 18
INT_NOERRCODE 19
INT_NOERRCODE 20
INT_NOERRCODE 21
INT_NOERRCODE 22
INT_NOERRCODE 23
INT_NOERRCODE 24
INT_NOERRCODE 25
INT_NOERRCODE 26
INT_NOERRCODE 27
INT_NOERRCODE 28
INT_NOERRCODE 29
INT_NOERRCODE 30
INT_NOERRCODE 31

; Remaining
%assign i 32
%rep 224
  INT_NOERRCODE i
  %assign i i+1
%endrep

;; Data
section .data

cpu_int_idtr:
  dw 0xFFF
  dq cpu_int_idt

global cpu_int_stubs
cpu_int_stubs:
  %assign i 0
  %rep 256
    dq cpu_int_stub %+ i
    %assign i i+1
  %endrep

;; BSS
section .bss

global cpu_int_idt
cpu_int_idt:
  resb 0x1000                   ; Reserve 4096 KB

global cpu_int_handlers
cpu_int_handlers:
  resb 0x1000                   ; Reserve 4096 KB
