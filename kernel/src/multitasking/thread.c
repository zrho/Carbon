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
#include <api/map.h>
#include <fpu.h>
#include <multitasking.h>
#include <memory.h>
#include <debug.h>
#include <ipc.h>

//- Thread ---------------------------------------------------------------------

#define THREAD_MAP(pid) ((thread_t **) (MEMORY_THREAD_MAP_VADDR + (pid * THREAD_MAP_SIZE)))

thread_t *thread_current = 0;
process_t *process_current = 0;

extern uint8_t idle;
static cpu_int_state_t _thread_state_idle;

static uint32_t _thread_id_next(uint32_t pid) {
    thread_t **map = THREAD_MAP(pid);
    uint32_t tid;

    for (tid = 0; tid < THREAD_MAX; ++tid)
        if (0 == map[tid])
            return tid;

    PANIC("Maximum number of threads reached.");
    return -1;
}

thread_t *thread_spawn(process_t *process, uintptr_t entry_point) {
    // Create thread structure
    thread_t *thread = (thread_t *) heap_alloc(sizeof(thread_t));
    memset(thread, 0, sizeof(thread_t));

    // Fill structure
    thread->tid = _thread_id_next(process->pid);
    thread->pid = process->pid;
    thread->frozen = 1;
    thread->entry_point = entry_point;
    stack_create(&thread->stack, process);
    thread->next_sched = 0;

    // Setup state
    thread->state.rsp = thread->state.state.rbp = (uintptr_t) thread->stack.address;
    thread->state.flags |= (1 << 9); // Enable interrupts
    thread->state.rip = entry_point;

    thread->state.cs = 0x1B;
    thread->state.ds = thread->state.ss = 0x23;

    // Setup FPU state
    // (512 bytes are allocated on 512 byte boundary, which is
    //  aligned to 16 bytes).
    thread->fx_data = heap_alloc(512);

    // Add to map
    THREAD_MAP(process->pid)[thread->tid] = thread;

    // Add to list
    thread->next = process->threads;
    process->threads = thread;

    return thread;
}

thread_t *thread_get(process_t *process, uint32_t tid) {
    if (tid >= THREAD_MAX)
        return 0;

    return THREAD_MAP(process->pid)[tid];
}

void thread_stop(process_t *process, thread_t *thread) {
    // Termination flag set?
    if (0 != (thread->flags & THREAD_FLAG_TERMINATED))
        return;

    // Is last thread and implicit thread-triggered termination flag
    // set in hosting process?
    if (process->threads == thread && 0 == thread->next &&
        0 != (process->term_implicit & PROCESS_TERM_THREADS)) {

        process_terminate(process->pid);
        return;
    }

    // Remove from scheduler
    thread_freeze(thread);

    // Dispose stack
    stack_dispose(&thread->stack, process);

    // Dispose FPU data
    heap_free(thread->fx_data);

    // Add terminated flag
    thread->flags |= THREAD_FLAG_TERMINATED;

    // Free message if it is an IPC thread
    if (THREAD_ROLE_IPC_RECEIVER == thread->role) {

        if (0 != thread->role_ctx) {
            heap_free(thread->role_ctx);
            thread->role_ctx = 0;
        }
    }

    // Dispose all IPC buffers
    uint8_t buffer;
    for (buffer = 0; buffer <= 1; ++buffer)
    	ipc_buffer_resize(0, buffer, thread);

    // Free if detached.
    if (0 != (thread->flags & THREAD_FLAG_DETACHED)) {
        thread_free(process, thread->tid);
        return;
    }

    // Search for threads that are awaiting to join with the thread
    thread_t *current = process->threads;

    while (0 != current) {
        if (0 == (current->flags & THREAD_FLAG_TERMINATED) &&
            THREAD_SLEEP_JOIN == current->sleep_mode &&
            *((uint32_t *) current->sleep_ctx) == thread->tid)
            thread_join_awake(current, thread);

        current = current->next;
    }
}

void thread_free(process_t *process, uint32_t tid) {
    // Find thread (does not use thread_get to be able to remove the thread easily)
    thread_t *thread = process->threads;
    thread_t *thread_prev = 0;

    while (0 != thread) {
        if (tid == thread->tid)
            break;

        thread_prev = thread;
        thread = thread->next;
    }

    // Thread found and terminated flag set?
    if (0 == thread || 0 == (thread->flags & THREAD_FLAG_TERMINATED))
        return;

    // Remove from map
    THREAD_MAP(process->pid)[tid] = 0;

    // Remove from process
    if (0 == thread_prev)
        process->threads = thread->next;
    else
        thread_prev->next = thread->next;

    // Free structure
    heap_free(thread);
}

void thread_dispose_all(process_t *process) {
    // Detach all running threads and stop them and
    // free all terminated threads
    thread_t *current = process->threads;

    while (0 != current) {
        // Backup next
        thread_t *next = current->next;

        // Still running?
        if (0 == (current->flags & THREAD_FLAG_TERMINATED)) {
            // Detach thread
            current->flags |= THREAD_FLAG_DETACHED;

            // Stop thread
            thread_stop(process, current);

        } else {
            // Free thread
            thread_free(process, current->tid);
        }

        // Continue with next
        current = next;
    }
}

void thread_freeze(thread_t *thread) {
    // Hasn't been frozen before?
    if (0 == thread->frozen++)
        // Remove from scheduler
        scheduler_remove(thread);
}

void thread_thaw(thread_t *thread, uint8_t flags) {
    // Already thawed?
    if (0 == thread->frozen)
        return;

    // Completely thawed afterwards?
    if (0 == --thread->frozen)
        // Add to scheduler
        scheduler_add(thread, SCHED_FLAG_THAWED | flags);
}

bool thread_join_sleep(thread_t *thread, thread_t *wait_for) {
    // Check process id
    if (UNLIKELY(thread->pid != wait_for->pid))
        PANIC("Trying to join with thread of foreign process.");

    // Check if detached
    if (UNLIKELY(0 != (wait_for->flags & THREAD_FLAG_DETACHED)))
        PANIC("Trying to join with detached thread.");

    // Do not sleep when already terminated
    if (UNLIKELY(0 != (wait_for->flags & THREAD_FLAG_TERMINATED)))
        return false;

    // Freeze thread
    thread_freeze(thread);

    // Set sleep mode
    thread->sleep_mode = THREAD_SLEEP_JOIN;
    thread->sleep_ctx = heap_alloc(sizeof(uint32_t));

    *((uint32_t *) thread->sleep_ctx) = wait_for->tid;

    return true;
}

void thread_join_awake(thread_t *thread, thread_t *wait_for) {
    // Check if sleeping because of join
    if (UNLIKELY(THREAD_SLEEP_JOIN != thread->sleep_mode))
        PANIC("Trying to awake non-joining thread from join-trigged sleep.");

    // Clear sleep mode
    thread->sleep_mode = 0;
    heap_free(thread->sleep_ctx);

    // Set result in thread's state
    thread->state.state.rbx = (uintptr_t) wait_for->result_ptr;

    // Thaw thread
    thread_thaw(thread, 0);
}

/*static void thread_state_dump(cpu_int_state_t *state) {
    DEBUG("Thread state dump\n");
    DEBUG("DS: ");
    DEBUG_HEX(state->ds);
    DEBUG("\nRIP: ");
    DEBUG_HEX(state->rip);
    DEBUG("\nCS: ");
    DEBUG_HEX(state->cs);
    DEBUG("\nSS: ");
    DEBUG_HEX(state->ss);
    DEBUG("\nFlags: ");
    DEBUG_HEX(state->flags);
    DEBUG("\nRSP: ");
    DEBUG_HEX(state->rsp);
    DEBUG("\n");
}*/

static void _thread_idle(cpu_int_state_t *state) {
    extern uint8_t stack_runtime_top;

    // No current thread or process
    process_current = 0;
    thread_current = 0;

    // Idle state initialized?
    if (0 == _thread_state_idle.rip) {
        _thread_state_idle.rsp = _thread_state_idle.state.rbp = (uintptr_t) &stack_runtime_top;
        _thread_state_idle.flags |= (1 << 9); // Enable interrupts
        _thread_state_idle.rip = (uintptr_t) &idle;

        _thread_state_idle.cs = 0x08;
        _thread_state_idle.ds = _thread_state_idle.ss = 0x10;
    }

    // Load state
    memcpy(state, &_thread_state_idle, sizeof(cpu_int_state_t));
}

void thread_switch(thread_t *thread, cpu_int_state_t *state) {
    // Backup state for current thread
    if (0 != thread_current) {
    	// Registers
        memcpy(&thread_current->state, state, sizeof(cpu_int_state_t));

        // FPU
        fpu_save(thread_current->fx_data);
    }

    // Idle?
    if (0 == thread) {
        _thread_idle(state);

    } else {
        // Get current process
        if (0 == process_current || process_current->pid != thread->pid)
            process_current = process_get(thread->pid);

        // Set current thread
        thread_current = thread;

        // Address space switch required?
        if (memory_space_get() != process_current->addr_space)
            memory_space_switch(process_current->addr_space);

        // Set thread state
        memcpy(state, &thread->state, sizeof(cpu_int_state_t));

        // Load FPU data
        fpu_load(thread_current->fx_data);

        // Is FPU state prepared?
        if (0 == (thread->flags & THREAD_FLAG_FX_PREPARED)) {
            fpu_prepare();
            thread->flags |= THREAD_FLAG_FX_PREPARED;
        }
    }
}
