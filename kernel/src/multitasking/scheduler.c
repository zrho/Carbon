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
#include <multitasking.h>
#include <debug.h>

//- Scheduler ------------------------------------------------------------------

static thread_t *scheduler_threads_first = 0;
static thread_t *scheduler_threads_last = 0;

void scheduler_add(thread_t *thread, uint8_t flags) {
    // Still frozen?
    if (UNLIKELY(thread->frozen > 0))
        PANIC("Trying to add frozen thread to scheduling.");
    
    // Instant
    if (0 != (flags & SCHED_FLAG_INSTANT)) {
        // Not recently thawed => already in scheduling
        // Otherwise will be added as first, anyway.
        if (0 == (flags & SCHED_FLAG_THAWED)) {
            // Remove from list, if not first
            if (thread != scheduler_threads_first)
                scheduler_remove(thread);
        }
    }
    
    // Add thread (as first in queue)
    thread->next_sched = scheduler_threads_first;
    scheduler_threads_first = thread;
    
    if (0 == scheduler_threads_last)
        scheduler_threads_last = thread;
}

void scheduler_remove(thread_t *thread) {
    // Iterate
    thread_t *thread_cur = scheduler_threads_first;
    thread_t *thread_prev = 0;

    while (0 != thread_cur) {
        // Match?
        if (thread->tid != thread_cur->tid ||
            thread->pid != thread_cur->pid) {
            thread_prev = thread_cur;
            thread_cur = thread_cur->next_sched;
            continue;
        }

        // First?
        if (thread_prev == 0)
            scheduler_threads_first = thread->next_sched;
        else
            thread_prev->next_sched = thread->next_sched;

        // Last?
        if (thread->next_sched == 0)
            scheduler_threads_last = thread_prev;

        break;
    }
}

thread_t *scheduler_next() {
    // No threads?
    if (0 == scheduler_threads_first)
        return 0;

    // One thread?
    if (scheduler_threads_first == scheduler_threads_last)
        return scheduler_threads_first;

    // Get next thread and add it to the end of the queue
    thread_t *next = scheduler_threads_first;
    scheduler_threads_first = next->next_sched;
    scheduler_threads_last->next_sched = next;
    scheduler_threads_last = next;
    
    return next;
}
