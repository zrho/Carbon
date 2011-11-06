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
#include <cpu.h>

//- System Call API ------------------------------------------------------------

/**
 * The interrupt vector reserved for system call invocation.
 */
#define SYSCALL_INT_VECTOR 0x80

/**
 * The number of defined system calls.
 */
#define SYSCALL_COUNT 55

/**
 * Type for system call handlers.
 *
 * @param state The state of the calling thread.
 */
typedef void (* syscall_handler_t)(cpu_int_state_t *);

/**
 * Handler for system calls invoked using the syscall interrupt.
 *
 * @param state The state of the calling thread. Can be changed to modify the
 *              return state.
 */
void syscall_handler_int(cpu_int_state_t *state);

//- System Call - Macros -------------------------------------------------------

#define SYSCALL_RETURN_ERROR(code) { \
    state->state.rax = code; \
    return; \
}

#define SYSCALL_RETURN_SUCCESS { \
    state->state.rax = 0; \
    return; \
}

#define SYSCALL_ROOT (0 == process_current->pid)

#define SYSCALL_SWITCH_THREAD thread_switch(scheduler_next(), state)

//- System Calls - Multitasking - Public ---------------------------------------

/**
 * System Call: Returns the current process's id.
 *
 * Output:
 *  * EBX The current process's id.
 */
void syscall_process_id(cpu_int_state_t *state);

/**
 * System Call: Returns the id of the parent process.
 *
 * Output:
 *   * ECX The id of the process.
 */
void syscall_process_parent_id(cpu_int_state_t *state);

/**
 * System Call: Kills the current process.
 */
void syscall_process_exit(cpu_int_state_t *state);

/**
 * System Call: Returns the current thread's id.
 *
 * Output:
 *  * EBX The current thread's id.
 */
void syscall_thread_id(cpu_int_state_t *state);

/**
 * System Call: Stops a thread of the current process.
 *
 * Input:
 *  * RBX The id of the thread to stop.
 *  * RSI Pointer to the thread's result.
 *  * RDX Reason:
 *         0 The thread has been stopped manually (default).
 *         1 The thread has returned.
 */
void syscall_thread_cancel(cpu_int_state_t *state);

/**
 * System Call: Sleeps to join with another thread.
 *
 * Fails when:
 *  * The awaited thread is equal to the invoking. [1]
 *  * The awaited thread does not exist. [2]
 *  * The awaited thread is detached. [3]
 *
 * Input:
 *  * RBX The id of the thread to join with.
 *
 * Output:
 *  * RAX Error code, zero on success.
 *  * RBX Pointer to result of thread.
 */
void syscall_thread_join(cpu_int_state_t *state);

/**
 * System Call: Spawns a thread for the current process.
 *
 * Input:
 *  * RDI Pointer to thread's entry point.
 *  * RSI Pointer to thread's arguments.
 *  * RDX Address the thread should return to.
 *
 * Output:
 *  * RAX Error code.
 *  * RBX Id of the newly created thread.
 */
void syscall_thread_spawn(cpu_int_state_t *state);

//- System Calls - Multitasking - Only Root ------------------------------------

/**
 * System Call: Kills a thread.
 *
 * Only Root Process.
 *
 * Input:
 *  * RBX The id of the thread.
 *  * RCX The id of the process.
 *  * RSI Pointer to the thread's result.
 *  * RDX Reason:
 *         0 The thread has been stopped manually (default).
 *         1 The thread has returned.
 *
 * Output:
 *  * RAX Error code.
 */
void syscall_thread_kill(cpu_int_state_t *state);

/**
 * System Call: Creates a thread.
 *
 * Only Root Process.
 *
 * Input:
 *  * RDI Pointer to thread's entry point.
 *  * RSI Pointer to thread's arguments.
 *  * RCX The id of the hosting thread.
 *  * RDX Address the thread should return to.
 *
 * Output
 *  * RAX Error code.
 *  * RBX The id of the newly created thread.
 */
void syscall_thread_create(cpu_int_state_t *state);

/**
 * System Call: Kills a process.
 *
 * Only Root Process.
 *
 * Input:
 *  * RCX The id of the thread to kill.
 *
 * Output:
 *  * RAX Error code.
 */
void syscall_process_kill(cpu_int_state_t *state);

/**
 * System Call: Creates a new process.
 *
 * Only Root Process.
 *
 * Input:
 *  * RCX The new process's parent process.
 *
 * Output:
 *  * RAX Error code.
 *  * RBX The new process's id.
 */
void syscall_process_create(cpu_int_state_t *state);

//- System Calls - Synchronization - Futex -------------------------------------

/**
 * System Call: Compares the futex with a given value and wakes a given number
 * or all threads waiting on it, when they are equal.
 *
 * Input:
 *  * RSI The address of the futex.
 *  * RBX The value to compare with.
 *  * RCX The number of threads to wake. (uint32_t) -1 for all.
 *
 * Output:
 *  * RAX 1 if values were equal, 0 if they weren't.
 */
void syscall_futex_wake(cpu_int_state_t *state);

/**
 * System Call: Compares the futex with a given value and, when they are equal,
 * waits on it until it is waked by a call to futex_wake.
 *
 * Input:
 *  * RSI The address of the futex.
 *  * RBX The value to compare with.
 *
 * Output:
 *  * RAX 1 if the values were equal and the thread has been waked up, 0 otherwise.
 */
void syscall_futex_wait(cpu_int_state_t *state);

//- System Calls - Synchronization - Mutex -------------------------------------

/**
 * System Call: Waits for a mutex to be released and locks it.
 *
 * Input:
 *  * RSI The address of the mutex to lock.
 *
 * Output:
 *  * RAX Error code.
 */
void syscall_mutex_lock(cpu_int_state_t *state);

/**
 * System Call: Locks a mutex if it isn't currently locked.
 *
 * Input:
 *  * RSI The address of the mutex to lock.
 *
 * Output:
 *  * RAX Error code.
 *  * RBX Zero if mutex had already been locked, one otherwise.
 */
void syscall_mutex_trylock(cpu_int_state_t *state);

/**
 * System Call: Unlocks a mutex and (by random) schedules a thread that has been
 * waiting for the mutex to be released.
 *
 * Input:
 *  * RSI The address of the mutex to unlock.
 *
 * Output:
 *  * RAX Error code.
 */
void syscall_mutex_unlock(cpu_int_state_t *state);

//- System Calls - IPC ---------------------------------------------------------

/**
 * System Call: Sends a message to the target process.
 *
 * Moves the message from the invoking thread's SEND buffer to the target
 * handler thread's RECV buffer.
 *
 * When the IGNORE_RESPONSE flag is not set, the response is written to the
 * thread's RECV buffer.
 *
 * Will return an error code, when the target process has no message handler.
 *
 * Input:
 *  * RDI The id of the target process.
 *  * RBX The flags for sending the message.
 *  * RCX The length of the message's payload (in bytes).
 *
 * Output:
 *  * RAX Error code.
 *  * RCX The size of the response.
 */
void syscall_ipc_send(cpu_int_state_t *state);

/**
 * System Call: Sends an response back the the sender of a message.
 *
 * Moves the message from the invoking thread's SEND buffer to the original
 * sender thread's RECV buffer.
 *
 * Requires the thread to have the THREAD_ROLE_RECEIVER role.
 *
 * Input:
 *  * RBX The flags for sending the response.
 *  * RCX The length of the response's payload.
 *
 * Output:
 *  * RAX Error code, when the thread does not have the right role.
 */
void syscall_ipc_respond(cpu_int_state_t *state);

/**
 * System Call: Resizes one of the current thread's IPC buffers.
 *
 * Input:
 *  * RBX The number of the IPC buffer to resize.
 *  * RCX The new size of the IPC buffer.
 *
 * Output:
 *  * RAX Error code.
 *  * RBX The address of the buffer.
 */
void syscall_ipc_buffer_size(cpu_int_state_t *state);

/**
 * System Call: Returns the address of one of the current thread's IPC buffers.
 *
 * Input:
 *  * RBX The number of the IPC buffer.
 *
 * Output:
 *  * RAX Error code.
 *  * RBX The address of the buffer.
 */
void syscall_ipc_buffer_get(cpu_int_state_t *state);

/**
 * System Call: Sets the message handler of the current process.
 *
 * Input:
 *  * RBX The address of the message handler (zero for none).
 *
 * Output:
 *  * RAX Error code.
 */
void syscall_ipc_handler(cpu_int_state_t *state);

//- System Calls - Memory ------------------------------------------------------

#define SYSCALL_MEMORY_FLAG_WRITEABLE (1 << 0)
#define SYSCALL_MEMORY_FLAG_NX (1 << 1)

/**
 * System Call: Allocates a free frame of physical memory.
 *
 * Only Root Process.
 *
 * Output:
 *  * RAX Error code, zero on success.
 *  * RBX Physical address of the frame.
 */
void syscall_memory_alloc(cpu_int_state_t *state);

/**
 * System Call: Frees a frame of physical memory.
 *
 * Only Root Process.
 *
 * Input:
 *  * RBX Physical address of the frame to free.
 *
 * Output:
 *  * RAX Error code.
 */
void syscall_memory_free(cpu_int_state_t *state);

/**
 * System Call: Maps the given virtual to the given physical address in the address
 * space of a process, given its pid.
 *
 * Only Root Process.
 *
 * Input:
 *  * RDI The target virtual address.
 *  * RSI The physical address of the frame to map.
 *  * RBX Page flags.
 *  * RCX The pid of the process.
 *
 * Output:
 *  * RAX Error code.
 */
void syscall_memory_map(cpu_int_state_t *state);

/**
 * System Call: Unmaps the given virtual address in the address space of a
 * process, given its pid.
 *
 * Only Root Process.
 *
 * Input:
 *  * RBX The address of the page to unmap.
 *  * RCX The pid of the process.
 *
 * Output:
 *  * RAX Error code.
 */
void syscall_memory_unmap(cpu_int_state_t *state);
