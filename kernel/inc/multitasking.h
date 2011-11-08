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

//- Constants ------------------------------------------------------------------

#define THREAD_ROLE_NORMAL          0
#define THREAD_ROLE_IPC_RECEIVER    1

#define THREAD_FLAG_TERMINATED      (1 << 0)
#define THREAD_FLAG_DETACHED        (1 << 1)
#define THREAD_FLAG_FX_PREPARED     (1 << 2)

#define THREAD_SLEEP_JOIN           1
#define THREAD_SLEEP_MUTEX		    2
#define THREAD_SLEEP_FUTEX          3

#define THREAD_TTL_GAIN             2

#define PROCESS_TERM_THREADS        (1 << 0)

//- Multitasking Structures ----------------------------------------------------

typedef struct stack_t {
    /**
     * The upper bound of the stack.
     */
    uintptr_t address;

    /**
     * The current length of the stack.
     */
    uintptr_t length;
} stack_t;

typedef struct thread_t {
    /**
     * The thread's id that is unique among the all the threads of the
     * hosting process.
     */
    uint32_t tid;

    /**
     * The pid of the hosting process.
     */
    uint32_t pid;
    
    /**
     * The times the thread is frozen.
     *
     * When completely thawed, the thread is added to scheduling.
     */
    uint8_t frozen;
    
    /**
     * Remaining time to live in ticks.
     */
    uint8_t ttl;

    /**
     * The role of the thread.
     */
    uint8_t role;

    /**
     * Context information about the thread's role.
     */
    void *role_ctx;

    /**
     * The reason why the thread sleeps.
     */
    uint8_t sleep_mode;

    /**
     * The context for the thread's sleeping state.
     */
    void *sleep_ctx;

    /**
     * Pointer to the thread's result.
     */
    void *result_ptr;

    /**
     * The sizes of the thread's IPC buffers.
     */
    uint32_t ipc_buffer_sz[3];

    /**
     * Flags describing the thread.
     */
    uint8_t flags;

    /**
     * The thread's entry point address.
     */
    uintptr_t entry_point;

    /**
     * Pointer to 512-byte data for SSE.
     */
    void *fx_data;

    /**
     * Backup of the thread's state since the last preemption.
     */
    cpu_int_state_t state;

    /**
     * The thread's stack.
     */
    stack_t stack;
    
    struct thread_t *next_sched;
    struct thread_t *next;
} thread_t;

typedef struct process_t {
    /**
     * The process's unique id.
     */
    uint32_t pid;

    /**
     * Pointer to the parent process.
     */
    struct process_t *parent;

    /**
     * The order of the process, i.e. the number of parent processes.
     *
     * Zero for the root process, one for its direct children etc.
     */
    uint16_t order;

    /**
     * The address of the process's message handler.
     */
    uintptr_t message_handler;

    /**
     * Rules for implicit termination.
     */
    uint8_t term_implicit;

    /**
     * The thread id that will be assigned to the next thread.
     */
    uint32_t tid_next;

    /**
     * The process's threads.
     */
    thread_t *threads;

    /**
     * The physical address of the process's address space structure.
     */
    uintptr_t addr_space;

    /**
     * The offset in the process's stack space.
     */
    uint64_t stack_offset;

    struct process_t *next;
} process_t;

//- Stack ----------------------------------------------------------------------

#define STACK_LENGTH_MAX (0x200000 - 0x1000) // -1 page as guard to prevent overflows
#define STACK_PROCESS_MAX 0x8000000000

void stack_create(stack_t *stack, process_t *process);
void stack_dispose(stack_t *stack, process_t *process);
void stack_resize(stack_t *stack, uintptr_t new_len, process_t *process);

//- Processes ------------------------------------------------------------------

process_t *process_list;
process_t *process_current;

process_t *process_spawn(uintptr_t addr_space, process_t *parent);
process_t *process_get(uint32_t pid);
void process_terminate(uint32_t pid);

//- Thread ---------------------------------------------------------------------

thread_t *thread_current;

thread_t *thread_spawn(process_t *process, uintptr_t entry_point);
thread_t *thread_get(process_t *process, uint32_t tid);

void thread_stop(process_t *process, thread_t *thread);
void thread_dispose_all(process_t *process);

void thread_free(process_t *process, uint32_t tid);

void thread_freeze(thread_t *thread);
void thread_thaw(thread_t *thread, uint8_t flags);

bool thread_join_sleep(thread_t *thread, thread_t *wait_for);
void thread_join_awake(thread_t *thread, thread_t *wait_for);

void thread_switch(thread_t *thread, cpu_int_state_t *state);

//- Scheduler ------------------------------------------------------------------

#define SCHED_FLAG_THAWED (1 << 0)
#define SCHED_FLAG_INSTANT (1 << 1)

void scheduler_add(thread_t *thread, uint8_t flags);
void scheduler_remove(thread_t *thread);

thread_t *scheduler_next(void);
thread_t *scheduler_current(void);
