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
#include <stdint.h>
#include <stdbool.h>
#include <carbon/process.h>

//- API - Multitasking - Thread ------------------------------------------------

#define THREAD_CANCEL_REASON_EXPLICIT 0
#define THREAD_CANCEL_REASON_RETURN   1

/**
 * Type for thread ids.
 */
typedef uint32_t tid_t;

/**
 * Returns the id of the current thread.
 *
 * @return Id of the current thread.
 */
tid_t thread_id(void);

/**
 * Waits for a thread to complete, given its id, and returns its result.
 *
 * @param tid The id of the thread to join with.
 * @return The thread's result or a null-pointer, if the thread does not
 *  exist or is detached.
 */
void *thread_join(tid_t tid);

/**
 * Kills a thread, given its id and the id of the hosting process.
 *
 * May be denied for processes other than the current (if not root).
 *
 * @param result The result of the thread.
 * @param reason The reason why the thread is killed.
 * @param tid The id of the thread to kill.
 * @param pid The id of the thread's host process.
 */
void thread_kill(void *result, uint8_t reason, tid_t tid, pid_t pid);

/**
 * Kills a thread in the current process.
 *
 * Same as:
 * <code>thread_kill(result, reason, tid, PROCESS_SELF);</code>
 *
 * @param result The result of the thread.
 * @param reason The reason why the thread is killed.
 * @param tid The id of the thread to kill.
 */
bool thread_cancel(void *result, uint8_t reason, tid_t tid);

/**
 * Kills the current thread.
 *
 * Same as:
 * <code>thread_kill(result, reason, THREAD_SELF, PROCESS_SELF);</code>
 *
 * @param result The result of the thread.
 * @param reason The reason why the thread is killed.
 */
void thread_exit(void *result, uint8_t reason) __attribute__((noreturn));

/**
 * Spawns a new thread, given the id of its new host process.
 *
 * May be denied for processes other than the current (if not root).
 *
 * @param entry The thread's entry point (in the host process's address space).
 * @param args Pointer to the thread's arguments (in the host process's
 *  address space).
 * @param ret The address the thread should return to.
 * @param pid The id of the host process.
 * @return The id of the newly created thread, or (tid_t) -1 if the thread
 *  could not be created.
 */
tid_t thread_create(void *entry, void *args, void *ret, pid_t pid);

/**
 * Spawns a new thread in the current process.
 *
 * Same as:
 * <code>thread_spawn_foreign(entry, args, PROCESS_SELF);</code>
 *
 * @param entry The thread's entry point.
 * @param args Pointer to the thread's arguments.
 * @param ret The address the thread should return to.
 * @return The id of the newly created thread.
 */
tid_t thread_spawn(void *entry, void *args, void *ret);
