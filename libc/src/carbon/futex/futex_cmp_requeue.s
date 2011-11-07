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

global futex_cmp_requeue:
futex_cmp_requeue:
	; Store
	push rbx
	push rcx
	push rdx
	push rsi
	push rdi
	push r8
	push r9
	push r10
	push r11
	push r12

	; System call number
	mov rax, 50

	; Parameters
	xchg r9, rdi
	xchg r10, rsi
	xchg r11, rdx
	xchg r12, rcx
	xchg r13, r8

	xchg rsi, r9
	xchg rbx, r10
	xchg rcx, r11
	xchg rdi, r12
	xchg rdx, r13

	; Call kernel
	int 0x80

	; Restore
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rdi
	pop rsi
	pop rdx
	pop rcx
	pop rbx
	ret
