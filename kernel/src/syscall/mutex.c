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
#include <multitasking.h>
#include <memory.h>

#include <debug.h>

//- System Calls - Synchronization - Mutex -------------------------------------

#define _CHECK_ACCESSIBLE(vaddr) \
	if (!memory_user_accessible(vaddr & ~0xFFF)) \
		SYSCALL_RETURN_ERROR(1);

void syscall_mutex_lock(cpu_int_state_t *state) {
	// Extract arguments
	uintptr_t mutex_vaddr = state->state.rdi;

	// Check
	_CHECK_ACCESSIBLE(mutex_vaddr);

	// Not locked yet?
	// Note that this is not multiprocessor and the kernel is not preemptible,
	// therefore no atomic operations are required here.
	uint8_t *mutex_ptr = (uint8_t *) mutex_vaddr;

	if (0 == *mutex_ptr) {
		// Lock mutex and return
		*mutex_ptr = 1;
		SYSCALL_RETURN_SUCCESS;
	}

	// Else, enter sleep state
	thread_freeze(thread_current);

	thread_current->sleep_mode = THREAD_SLEEP_MUTEX;
	thread_current->sleep_ctx = mutex_ptr;

	// Schedule next thread
	SYSCALL_SWITCH_THREAD;
	SYSCALL_RETURN_SUCCESS;
}

void syscall_mutex_trylock(cpu_int_state_t *state) {
	// Extract arguments
	uintptr_t mutex_vaddr = state->state.rdi;

	// Check
	_CHECK_ACCESSIBLE(mutex_vaddr);

	// Not locked yet?
	uint8_t *mutex_ptr = (uint8_t *) mutex_vaddr;
	uint64_t success = 0;

	if (0 == *mutex_ptr) {
		// Lock mutex and set success value
		*mutex_ptr = 1;
		success = 1;
	}

	// Return whether the mutex has been locked successfully
	state->state.rbx = success;
	SYSCALL_RETURN_SUCCESS;
}

void syscall_mutex_unlock(cpu_int_state_t *state) {
	// Extract arguments
	uintptr_t mutex_vaddr = state->state.rdi;

	// Check
	_CHECK_ACCESSIBLE(mutex_vaddr);

	// Not currently locked?
	uint8_t *mutex_ptr = (uint8_t *) mutex_vaddr;

	if (0 == *mutex_ptr) {
		// No further action required
		SYSCALL_RETURN_SUCCESS;
	}

	// Else, try to find a thread that waits for the mutex
	// to be unlocked
	thread_t *thread = process_current->threads;

	while (0 != thread) {
		// Waiting for mutex?
		if (THREAD_SLEEP_MUTEX == thread->sleep_mode &&
			mutex_ptr == thread->sleep_ctx) {
			// Thaw thread and keep mutex locked
			thread->sleep_mode = 0;
			thread->sleep_ctx = 0;

			thread_thaw(thread, 0);
			SYSCALL_RETURN_SUCCESS;
		}

		// Next thread
		thread = thread->next;
	}

	// Unlock mutex
	*mutex_ptr = 0;
	SYSCALL_RETURN_SUCCESS;
}
