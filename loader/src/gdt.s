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
section .data

; Lower-half 64 bit GDT pointer
align 4096
global gdtr64
global gdt64

; GDT pointer
gdtr64:
  dw 0x27                       ; Length of 0x27
  dd gdt64                      ; Address lower DWORD
  dd 0x0                        ; Address upper DWORD

gdt64:
  .Null:
    dd 0x0
    dd 0x0
  .KernelCode:
    dd 0x0
    dd 0x209800
  .KernelData:
    dd 0x0
    dd 0x209200
  .UserCode:
    dd 0x0
    dd 0x20F800
  .UserData:
    dd 0x0
    dd 0x20F200
