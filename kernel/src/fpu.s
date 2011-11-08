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

global fpu_init
fpu_init:
	; Clear CR.EM and set CR.MP
	mov rax, cr0
	and rax, ~(1 << 2)
	or rax, (1 << 1)
	mov cr0, rax

	; Set CR4.OSFXSR and CR4.OSXMMEXCPT
	mov rax, cr4
	or rax, (1 << 9)
	or rax, (1 << 10)
	mov cr4, rax

	ret

global fpu_prepare
fpu_prepare:
	; Set MXCSR.PM, MXCSR.UM, MXCSR.OM,
	;     MXCSR.ZM and MXCSR.DM
	stmxcsr dword [rsp - 4]
	mov eax, dword [rsp - 4]
	or rax, (1 << 12) | (1 << 11) | (1 << 10) | (1 << 9) | (1 << 8)
	mov dword [rsp - 4], eax
	ldmxcsr dword [rsp - 4]
	ret

global fpu_save:
fpu_save:
	fxsave [rdi]
	ret

global fpu_load
fpu_load:
	fxrstor [rdi]
	ret
