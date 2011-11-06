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
#include <syscall.h>
#include <debug.h>
#include <multitasking.h>
#include <memory.h>

//- System Calls - Multitasking - Common ---------------------------------------

static void _syscall_thread_kill(
		uint8_t reason,
		uintptr_t result,
		uint32_t tid,
		process_t *process,
		cpu_int_state_t *state) {

	// Try to get thread
	thread_t *thread = thread_get(process, tid);

	if (0 == thread)
		SYSCALL_RETURN_ERROR(1);

	// Check if main thread
	bool main_thread = thread->tid == 0;

	// Check if thread switch is required
	bool current_proc = thread->pid == process_current->pid;
	bool current_thread = thread->tid == thread_current->tid;
	bool switch_thread = current_proc && current_thread;

	// Set the threads result
	thread->result_ptr = (void *) result;

	// Stop the thread
	thread_stop(process, thread);

	// If the main thread returned, terminate the process
	if (main_thread && 1 == reason) {
		process_terminate(process->pid);

		// Perform a thread switch, as the current process does
		// not exist anymore
		if (current_proc) switch_thread = true;
	}

	// Switch thread
	if (switch_thread)
		SYSCALL_SWITCH_THREAD;
	else
		SYSCALL_RETURN_SUCCESS;
}

static void _syscall_thread_create(
		uintptr_t entry_point,
		uintptr_t args,
		uintptr_t ret,
		process_t *process,
		cpu_int_state_t *state) {

    // Spawn new thread
    thread_t *thread = thread_spawn(process, entry_point);

    // Write return address to thread's stack
    if (process->addr_space != process_current->addr_space)
        memory_space_switch(process->addr_space);

    stack_resize(&thread->stack, 0x1000, process);
    uintptr_t *ptr = (uintptr_t *) (thread->stack.address - sizeof(uintptr_t));
    *ptr = ret;

    if (process->addr_space != process_current->addr_space)
        memory_space_switch(process_current->addr_space);

    thread->state.rsp -= 0x8;

    // Argument pointer in rdi register
    thread->state.state.rdi = args;

    // Thread's id
    state->state.rbx = thread->tid;

    // Thaw thread
    thread_thaw(thread, 0);

    SYSCALL_RETURN_SUCCESS;
}

//- System Calls - Multitasking - Public ---------------------------------------

void syscall_process_id(cpu_int_state_t *state) {
    state->state.rbx = process_current->pid;
    SYSCALL_RETURN_SUCCESS;
}

void syscall_thread_id(cpu_int_state_t *state) {
    state->state.rbx = thread_current->tid;
    SYSCALL_RETURN_SUCCESS;
}

void syscall_process_parent_id(cpu_int_state_t *state) {
    // Return parent of process
    if (0 != process_current->parent)
        state->state.rbx = process_current->parent->pid;
    else
        state->state.rbx = -1;

    SYSCALL_RETURN_SUCCESS;
}

void syscall_thread_cancel(cpu_int_state_t *state) {
    // Extract arguments
	uint32_t tid = (uint32_t) state->state.rbx;
    uint8_t reason = (uint8_t) state->state.rdx;
    uintptr_t result = state->state.rsi;

    // Call common implementation
    _syscall_thread_kill(reason, result, tid, process_current, state);
}

void syscall_thread_join(cpu_int_state_t *state) {
    // Extract arguments
    uint32_t awaited_thread_id = (uint32_t) state->state.rbx;

    // Check if the thread is equal to the invoking
    if (awaited_thread_id == thread_current->tid)
        SYSCALL_RETURN_ERROR(1);

    // Check if the thread exists
    thread_t *awaited_thread = thread_get(process_current, awaited_thread_id);

    if (0 == awaited_thread)
        SYSCALL_RETURN_ERROR(2);

    // Check if the thread is detached
    if (0 != (awaited_thread->flags & THREAD_FLAG_DETACHED))
        SYSCALL_RETURN_ERROR(3);

    // Sleep for join
    if (!thread_join_sleep(thread_current, awaited_thread)) {
    	// Thread already terminated, result therefore is already there
    	state->state.rbx = (uintptr_t) awaited_thread->result_ptr;

    } else {
    	// Switch thread
		SYSCALL_SWITCH_THREAD;
    }
}

void syscall_process_exit(cpu_int_state_t *state) {
    // Terminate the current process
    process_terminate(process_current->pid);

    // Switch thread
	SYSCALL_SWITCH_THREAD;

    SYSCALL_RETURN_SUCCESS;
}

void syscall_thread_spawn(cpu_int_state_t *state) {
	// Extract arguments
	uintptr_t entry_point = state->state.rdi;
	uintptr_t args = state->state.rsi;
	uintptr_t ret = state->state.rdx;

	// Call common implementation
	_syscall_thread_create(entry_point, args, ret, process_current, state);
}

//- System Calls - Multitasking - Only Root ------------------------------------

void syscall_thread_kill(cpu_int_state_t *state) {
	// Check permissions
	if (!SYSCALL_ROOT)
		SYSCALL_RETURN_ERROR(1);

	// Extract arguments
	uint32_t tid = (uint32_t) state->state.rbx;
	uint32_t pid = (uint32_t) state->state.rcx;
	uint8_t reason = (uint8_t) state->state.rdx;
	uintptr_t result = state->state.rsi;

	// Get process
	process_t *process = process_get(pid);

	if (0 == process)
		SYSCALL_RETURN_ERROR(2);

	// Call common implementation
	_syscall_thread_kill(reason, result, tid, process, state);
}

void syscall_thread_create(cpu_int_state_t *state) {
	// Check permissions
	if (!SYSCALL_ROOT)
		SYSCALL_RETURN_ERROR(1);

	// Extract arguments
	uintptr_t entry_point = state->state.rdi;
	uintptr_t args = state->state.rsi;
	uint32_t pid = state->state.rcx;
	uintptr_t ret = state->state.rdx;

	// Get process
	process_t *process = process_get(pid);

	if (0 == process)
		SYSCALL_RETURN_ERROR(2);

	// Call common implementation
	_syscall_thread_create(entry_point, args, ret, process, state);
}

void syscall_process_kill(cpu_int_state_t *state) {
	// Check permissions
	if (!SYSCALL_ROOT)
		SYSCALL_RETURN_ERROR(1);

	// Extract arguments
	uint32_t pid = state->state.rcx;

	// Check whether to switch the thread afterwards, due to
	// termination of the current process.
	bool switch_thread = process_current->pid == pid;

	// Terminate the process
	process_terminate(pid);

	if (switch_thread)
		SYSCALL_SWITCH_THREAD;
	else
		SYSCALL_RETURN_SUCCESS;
}

void syscall_process_create(cpu_int_state_t *state) {
	// Check permissions
	if (!SYSCALL_ROOT)
		SYSCALL_RETURN_ERROR(1);

	// Extract arguments
	uint32_t parent_pid = state->state.rcx;

	// Get parent process
	process_t *parent_proc = process_get(parent_pid);

	if (0 == parent_proc)
		SYSCALL_RETURN_ERROR(2);

	// Spawn new process
	process_t *proc = process_spawn(memory_space_create(), parent_proc);

	// Return the new process's pid
	state->state.rbx = proc->pid;

	SYSCALL_RETURN_SUCCESS;
}
