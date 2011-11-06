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

extern thread_id
extern thread_cancel

global thread_exit
thread_exit:
	; Backup result and reason
	push rdi
	push rsi

	; Get current tid
	mov rax, thread_id
	call rax

	; Get result and reason
	pop rsi
	pop rdi

	; Call thread_cancel
	xchg rdx, rax
	mov rax, thread_cancel
	call rax

	; Won't ever return
	jmp $
