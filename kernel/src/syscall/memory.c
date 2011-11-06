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
#include <api/string.h>

#include <syscall.h>
#include <debug.h>
#include <multitasking.h>
#include <memory.h>

//- System Calls - Memory ------------------------------------------------------

void syscall_memory_alloc(cpu_int_state_t *state) {
    // Check permissions
	if (!SYSCALL_ROOT)
		SYSCALL_RETURN_ERROR(1);

	// Return new frame
    state->state.rbx = frame_alloc();

    SYSCALL_RETURN_SUCCESS;
}

void syscall_memory_free(cpu_int_state_t *state) {
	// Check permissions
	if (!SYSCALL_ROOT)
		SYSCALL_RETURN_ERROR(1);

	// Free a given frame
    frame_free(state->state.rbx);

    SYSCALL_RETURN_SUCCESS;
}

void syscall_memory_map(cpu_int_state_t *state) {
	// Check permissions
	if (!SYSCALL_ROOT)
		SYSCALL_RETURN_ERROR(1);

    // Extract parameters
    uintptr_t virt = memalign(state->state.rdi, 0x1000);
    uintptr_t phys = memalign(state->state.rsi, 0x1000);
    uint16_t flags = (uint16_t) state->state.rbx;
    uint32_t pid = (uint32_t) state->state.rcx;

    // Check if the process exists
    process_t *proc = process_get(pid);

    if (0 == proc)
        SYSCALL_RETURN_ERROR(2);

    // Translate flags
    uint16_t pflags = PAGE_FLAG_USER;

    if (0 != (flags & SYSCALL_MEMORY_FLAG_WRITEABLE))
        pflags |= PAGE_FLAG_WRITEABLE;

    // Switch address space
    uintptr_t old_space = memory_space_switch(proc->addr_space);

    // Map page
    memory_map(virt, phys, pflags);

    // Switch back
    memory_space_switch(old_space);

    SYSCALL_RETURN_SUCCESS;
}

void syscall_memory_unmap(cpu_int_state_t *state) {
	// Check permissions
	if (!SYSCALL_ROOT)
		SYSCALL_RETURN_ERROR(1);

    // Extract parameters
    uintptr_t virt = memalign(state->state.rbx, 0x1000);
    uint32_t pid = (uint32_t) state->state.rcx;

    // Check if the process exists
    process_t *proc = process_get(pid);

    if (0 == proc)
        SYSCALL_RETURN_ERROR(2);

    // Switch address space
    uintptr_t old_space = memory_space_switch(proc->addr_space);

    // Unmap page
    memory_unmap(virt);

    // Switch back
    memory_space_switch(old_space);

    SYSCALL_RETURN_SUCCESS;
}
