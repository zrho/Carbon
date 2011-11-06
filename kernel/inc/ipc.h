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

#pragma once
#include <api/types.h>
#include <multitasking.h>

//- IPC - Structures -----------------------------------------------------------

#define IPC_FLAG_RESPONSE (1 << 0)
#define IPC_FLAG_IGNORE_RESPONSE (1 << 1)

/**
 * Role context structure for message handlers to track the
 * process and thread to respond to.
 */
typedef struct ipc_role_ctx_t {
	/**
	 * The id of the sender process.
	 */
    uint32_t sender_process;

    /**
     * The id of the sender thread.
     */
    uint32_t sender_thread;

    /**
     * The flags of the message.
     */
    uint16_t flags;

} ipc_role_ctx_t;

//- IPC - Buffer ----------------------------------------------------------------

#define IPC_BUFFER_SEND 0
#define IPC_BUFFER_RECV 1

#define IPC_BUFFER_VADDR(buf) \
		((buf == IPC_BUFFER_SEND) \
			? MEMORY_IPC_SEND_BUFFER_VADDR \
			: MEMORY_IPC_RECV_BUFFER_VADDR)

#define IPC_BUFFER_SIZE 0x200000

/**
 * Resizes a thread's IPC buffer.
 *
 * Requires to be in the thread's address space.
 *
 * @param size The new size of the IPC buffer.
 * @param buffer The number of the buffer.
 * @param thread The thread whose IPC buffer to resize.
 */
void ipc_buffer_resize(uint32_t size, uint8_t buffer, thread_t *thread);

/**
 * Moves an IPC buffer from the source thread to the target thread.
 *
 * Will return in the target thread's address space.
 *
 * @param source The source thread whose IPC buffer to move.
 * @param soruce_buf The number of the buffer to move.
 * @param target The target thread.
 * @param target_buf The number of the buffer to move to.
 * @param target_proc The process containing the target thread.
 */
void ipc_buffer_move(
		thread_t *source, uint8_t source_buf,
		thread_t *target, uint8_t target_buf,
		process_t *target_proc);

//- IPC ------------------------------------------------------------------------

/**
 * Sets the header registers in the given state.
 *
 * @param buffer The buffer the message's payload has been written to.
 * @param length The length of the message's payload.
 * @param flags The message's flags.
 * @param sender_pid The pid of the sender process.
 * @param thread_id The id of the thread for which the header should be set.
 * @param state The state in which to set the header registers.
 */
void ipc_message_header(
		uint8_t buffer,
		uint32_t length,
		uint16_t flags,
		uint32_t sender_pid,
		uint32_t thread_id,
		cpu_int_state_t *state);
