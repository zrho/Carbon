/**
 * Carbon Operating System
 * Copyright (C) 2011 Lukas Heidemann
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <api/types.h>
#include <ipc.h>
#include <syscall.h>
#include <memory.h>
#include <debug.h>

//- System Calls - IPC ---------------------------------------------------------

void syscall_ipc_send(cpu_int_state_t *state) {
	// Extract arguments
	uint32_t pid = (uint32_t) state->state.rdi;
	uint16_t flags = (uint32_t) state->state.rbx;
	uint32_t length = (uint32_t) state->state.rcx;

	// Check if process exists
	process_t *process_target = (pid == process_current->pid)
			? process_current
			: process_get(pid);

	if (0 == process_target)
		SYSCALL_RETURN_ERROR(1);

	// Check handler
	if (0 == process_target->message_handler)
		SYSCALL_RETURN_ERROR(2);

	// Check buffer size
	if (length > thread_current->ipc_buffer_sz[IPC_BUFFER_SEND])
		SYSCALL_RETURN_ERROR(3);

	// Spawn handler thread
	thread_t *handler = thread_spawn(
			process_target,
			process_target->message_handler);

	// Set thread role
	ipc_role_ctx_t *role_ctx =
			(ipc_role_ctx_t *) heap_alloc(sizeof(ipc_role_ctx_t));

	role_ctx->flags = flags;
	role_ctx->sender_process = process_current->pid;
	role_ctx->sender_thread = thread_current->tid;

	handler->role = THREAD_ROLE_IPC_RECEIVER;
	handler->role_ctx = role_ctx;

	// Move buffer to handler thread
	if (length > 0)
		ipc_buffer_move(
				thread_current,
				IPC_BUFFER_SEND,
				handler,
				IPC_BUFFER_RECV,
				process_target);

	// Write header to registers
	ipc_message_header(
			IPC_BUFFER_RECV,
			length,
			flags,
			process_current->pid,
			handler->tid,
			&handler->state);

	// Freeze the invoking thread, if a response is expected
	if (0 == (flags & IPC_FLAG_IGNORE_RESPONSE))
		thread_freeze(thread_current);

	// Switch to handler thread
	thread_switch(handler, state);
}

void syscall_ipc_respond(cpu_int_state_t *state) {
	// Check thread role
	if (THREAD_ROLE_IPC_RECEIVER != thread_current->role)
		SYSCALL_RETURN_ERROR(1);

	// Extract arguments
	uint16_t flags = (uint16_t) state->state.rbx;
	uint32_t length = (uint32_t) state->state.rcx;

	// Check length
	if (length > thread_current->ipc_buffer_sz[IPC_BUFFER_SEND])
		SYSCALL_RETURN_ERROR(2);

	// Extract info from role ctx
	ipc_role_ctx_t *role_ctx = (ipc_role_ctx_t *) thread_current->role_ctx;
	uint32_t sender_pid = role_ctx->sender_process;
	uint32_t sender_tid = role_ctx->sender_thread;

	// Sender process still exists?
	process_t *sender_process = process_get(sender_pid);

	if (0 == sender_process) {
		thread_stop(process_current, thread_current);
		thread_switch(scheduler_next(), state);
		return;
	}

	// Sender thread still exists?
	thread_t *sender_thread = thread_get(sender_process, sender_tid);

	if (0 == sender_thread) {
		thread_stop(process_current, thread_current);
		thread_switch(scheduler_next(), state);
		return;
	}

	// Response ignored?
	if (0 != (role_ctx->flags & IPC_FLAG_IGNORE_RESPONSE)) {
		thread_stop(process_current, thread_current);
		thread_switch(sender_thread, state);
		return;
	}

	// Move buffer to sender thread (if length > 0)
	if (length > 0)
		ipc_buffer_move(
				thread_current,
				IPC_BUFFER_SEND,
				sender_thread,
				IPC_BUFFER_RECV,
				sender_process);

	// Write header to registers
	ipc_message_header(
			IPC_BUFFER_RECV,
			length,
			flags,
			process_current->pid,
			sender_thread->tid,
			&sender_thread->state);

	// Thaw thread
	thread_thaw(sender_thread, 0);

	// Stop current thread and switch to sender
	thread_stop(process_current, thread_current);
	thread_switch(sender_thread, state);
}

void syscall_ipc_buffer_size(cpu_int_state_t *state) {
	// Extract arguments
	uint8_t buffer = state->state.rbx;
	uint32_t size = (uint32_t) state->state.rcx;

	// Check buffer
	if (buffer > 2)
		SYSCALL_RETURN_ERROR(1);

	// Check size
	if (size > IPC_BUFFER_SIZE)
		SYSCALL_RETURN_ERROR(2);

	// Resize buffer
	ipc_buffer_resize(size, buffer, thread_current);

	// Return address
	state->state.rbx = IPC_BUFFER_VADDR(buffer) +
			IPC_BUFFER_SIZE * thread_current->tid;

	SYSCALL_RETURN_SUCCESS;
}

void syscall_ipc_buffer_get(cpu_int_state_t *state) {
	// Extract arguments
	uint8_t buffer = state->state.rbx;

	// Check buffer
	if (buffer > 2)
		SYSCALL_RETURN_ERROR(1);

	// Return address
	state->state.rbx = IPC_BUFFER_VADDR(buffer) +
			IPC_BUFFER_SIZE * thread_current->tid;

	SYSCALL_RETURN_SUCCESS;
}

void syscall_ipc_handler(cpu_int_state_t *state) {
	// Extract arguments
	uintptr_t handler = state->state.rbx;

	// Set handler
	process_current->message_handler = handler;

	SYSCALL_RETURN_SUCCESS;
}
