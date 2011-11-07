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

#include <cpu.h>
#include <syscall.h>
#include <debug.h>

//- System Call API ------------------------------------------------------------

static void syscall_debug(cpu_int_state_t *state) {
	DEBUG((int8_t *) state->state.rbx);
	SYSCALL_RETURN_SUCCESS;
}

static void syscall_debug_hex(cpu_int_state_t *state) {
	DEBUG_HEX(state->state.rbx);
	SYSCALL_RETURN_SUCCESS;
}

static syscall_handler_t _syscall_handlers[] = {
        // 0 - 7
        &syscall_process_id,
        &syscall_process_parent_id,
        &syscall_process_exit,
        &syscall_thread_id,
        &syscall_thread_spawn,
        &syscall_thread_join,
        &syscall_thread_cancel,
        0,

        // 8 - 15
        &syscall_process_create,
        &syscall_process_kill,
        &syscall_thread_create,
        &syscall_thread_kill,
        0, 0, 0, 0,

        // 16 - 23
        &syscall_mutex_lock,
        &syscall_mutex_unlock,
        &syscall_mutex_trylock,
        0, 0, 0, 0, 0,

        // 24 - 31
        &syscall_ipc_send,
        &syscall_ipc_respond,
        &syscall_ipc_buffer_size,
        &syscall_ipc_buffer_get,
        &syscall_ipc_handler,
        0, 0, 0,

        // 32 - 39
        &syscall_memory_alloc,
        &syscall_memory_free,
        &syscall_memory_map,
        &syscall_memory_unmap,
        0, 0, 0, 0,

        // 40 - 47
        &syscall_debug,
        &syscall_debug_hex,
        0, 0, 0, 0, 0, 0,

        // 48 - 55
        &syscall_futex_wake,
        &syscall_futex_wait,
        &syscall_futex_cmp_requeue,
        0, 0, 0, 0, 0
};

void syscall_handler_int(cpu_int_state_t *state) {
    // Check system call number
    uint64_t number = state->state.rax;

    if (UNLIKELY(number > SYSCALL_COUNT) || 0 == _syscall_handlers[number]) {
        // Log debug message and return
        DEBUG("Unknown system call ");
        DEBUG_HEX(number);
        DEBUG("\n");

        return;
    }

    // Get handler
    syscall_handler_t handler = _syscall_handlers[number];
    handler(state);
}
