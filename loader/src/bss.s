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
section .bss

global stack_begin
global stack_end
global boot_pml4
global boot_info
global boot_info_end

;; The stack to be used by the loader.
align 4096

stack_begin:
  resb 0x1000
stack_end:

;; The initial pml4
align 4096
global boot_pml4

boot_pml4:
  resb 0x1000

;; The boot info
global boot_info
global boot_info_end

boot_info:
  resb 0x1000
boot_info_end:
