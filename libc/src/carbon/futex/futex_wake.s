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

global futex_wake:
futex_wake:
	; Store
	push rbx
	push rcx
	push rsi
	push r8
	push r9
	push r10

	; System call number
	mov rax, 48

	; Parameters
	xchg r8, rdi
	xchg r9, rsi
	xchg r10, rdx

	xchg rsi, r8
	xchg rbx, r9
	xchg rcx, r10

	; Call kernel
	int 0x80

	; Result and Restore
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rcx
	pop rbx
	ret
