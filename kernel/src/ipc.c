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
#include <api/compiler.h>
#include <api/string.h>

#include <ipc.h>
#include <debug.h>
#include <multitasking.h>
#include <memory.h>

//- IPC - Buffer ----------------------------------------------------------------

void ipc_buffer_resize(uint32_t size, uint8_t buffer, thread_t *thread) {
	// Align size
	size = memalign(size, 0x1000);

	// Greater than maximum size?
	if (UNLIKELY(size > IPC_BUFFER_SIZE))
		PANIC("Trying to increase IPC buffer size to a size larger than maximum.");

	// Get current size
	uint32_t size_current = thread->ipc_buffer_sz[buffer];

	// Get buffer address
	uintptr_t buffer_addr =
			IPC_BUFFER_VADDR(buffer) +
			thread->tid * IPC_BUFFER_SIZE;

	// Size increased?
	if (size > size_current) {
		// Map new pages
		uintptr_t addr = buffer_addr + size_current;

		for (; addr < buffer_addr + size; addr += 0x1000)
			memory_map(addr, frame_alloc(), PAGE_FLAG_USER | PAGE_FLAG_WRITEABLE);

	} else if (size < size_current) {
		// Remove unused pages
		uintptr_t addr = buffer_addr + size;

		for (; addr < buffer_addr + size_current; addr += 0x1000) {
			uintptr_t phys = memory_physical(addr);
			memory_unmap(addr);
			frame_free(phys);
		}
	}

	// Update size
	thread->ipc_buffer_sz[buffer] = size;
}

void ipc_buffer_move(
		thread_t *source, uint8_t source_buf,
		thread_t *target, uint8_t target_buf,
		process_t *target_proc) {
	// Get PT in source address space
    uintptr_t source_addr =
    		IPC_BUFFER_VADDR(source_buf) + IPC_BUFFER_SIZE * source->tid;
    uint64_t pt = memory_struct_remove(source_addr, PAGE_STRUCT_PT);

    // Switch to the target's address space
    memory_space_switch(target_proc->addr_space);

    // Map the message to the target buffer
    uint64_t target_addr =
    		IPC_BUFFER_VADDR(target_buf) + IPC_BUFFER_SIZE * target->tid;

    memory_struct_insert(target_addr, PAGE_STRUCT_PT, pt);

    // Change buffer sizes
    target->ipc_buffer_sz[target_buf] = source->ipc_buffer_sz[source_buf];
    source->ipc_buffer_sz[source_buf] = 0;
}

//- IPC ------------------------------------------------------------------------

void ipc_message_header(
		uint8_t buffer,
		uint32_t length,
		uint16_t flags,
		uint32_t sender_pid,
		uint32_t thread_id,
		cpu_int_state_t *state) {

    state->state.rdi =
    		IPC_BUFFER_VADDR(buffer) + IPC_BUFFER_SIZE * thread_id;
    state->state.rsi = length;
    state->state.rdx = sender_pid;
    state->state.rbx = flags;
}
