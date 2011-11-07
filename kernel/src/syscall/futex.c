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

static void _futex_wake_threads(uint32_t thread_count, uintptr_t futex_vaddr) {
    thread_t *thread = process_current->threads;

    while (0 != thread && thread_count > 0) {
        // Is sleeping because of this futex?
        if (thread->sleep_mode == THREAD_SLEEP_FUTEX &&
            (uintptr_t) thread->sleep_ctx == futex_vaddr) {
            // Wake thread up
            thread->sleep_mode = 0;
            thread_thaw(thread, 0);

            // Decrease number of remaining threads
            --thread_count;
        }

        // Next
        thread = thread->next;
    }
}

void syscall_futex_wake(cpu_int_state_t *state) {
    // Extract arguments
    uintptr_t futex_vaddr = state->state.rsi;
    uint32_t thread_count = (uint32_t) state->state.rcx;

    // Check
    if (!memory_user_accessible(futex_vaddr & ~0xFFF)) {
        state->state.rax = 0;
        return;
    }

    // Wake n threads
    _futex_wake_threads(thread_count, futex_vaddr);

    // Values were equal
    state->state.rax = 1;
    return;
}

void syscall_futex_wait(cpu_int_state_t *state) {
    // Extract arguments
    uintptr_t futex_vaddr = state->state.rsi;
    uint32_t value_cmp = (uint32_t) state->state.rbx;

    // Check
    if (!memory_user_accessible(futex_vaddr & ~0xFFF)) {
        state->state.rax = 0;
        return;
    }

    // Compare values
    uint32_t *futex = (uint32_t *) futex_vaddr;

    if (value_cmp != *futex) {
        state->state.rax = 0;
        return;
    }

    // Enter sleep
    thread_current->sleep_mode = THREAD_SLEEP_FUTEX;
    thread_current->sleep_ctx = (void *) futex_vaddr;
    thread_freeze(thread_current);

    // Ensure that 1 is returned on wakeup
    state->state.rax = 1;

    // Switch threads
    SYSCALL_SWITCH_THREAD;
}

void syscall_futex_cmp_requeue(cpu_int_state_t *state) {
    // Extract arguments
    uintptr_t futex_vaddr = state->state.rsi;
    uintptr_t target_vaddr = state->state.rdi;
    uint32_t value_cmp = (uint32_t) state->state.rbx;
    uint32_t wake_count = (uint32_t) state->state.rcx;
    uint32_t transfer_count = (uint32_t) state->state.rdx;

    // Check
    if (!memory_user_accessible(futex_vaddr & ~0xFFF)) {
        state->state.rax = 0;
        return;
    }

    // Compare values
    uint32_t *futex = (uint32_t *) futex_vaddr;

    if (value_cmp != *futex) {
        state->state.rax = 0;
        return;
    }

    // Wake up threads
    _futex_wake_threads(wake_count, futex_vaddr);

    // Transfer threads
    thread_t *thread = process_current->threads;

    while (0 != thread && transfer_count > 0) {
        // Is waiting on futex?
        if (thread->sleep_mode == THREAD_SLEEP_FUTEX &&
            thread->sleep_ctx == (void *) futex_vaddr) {
            // Transfer thread
            thread->sleep_ctx = (void *) target_vaddr;
            --transfer_count;
        }

        // Next
        thread = thread->next;
    }

    // Success
    state->state.rax = 1;
    return;
}
