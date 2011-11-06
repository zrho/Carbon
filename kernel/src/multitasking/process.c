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
#include <multitasking.h>
#include <memory.h>
#include <debug.h>

//- Processes ------------------------------------------------------------------

static uint32_t process_pid_last = 0;
process_t *process_list = 0;

process_t *process_spawn(uintptr_t addr_space, process_t *parent) {
    // Allocate process structure
    process_t *proc = (process_t *) heap_alloc(sizeof(process_t));
    memset(proc, 0, sizeof(process_t));

    // Fill structure
    proc->pid = process_pid_last++;
    proc->threads = 0;
    proc->addr_space = addr_space;
    proc->stack_offset = 0;
    proc->parent = parent;

    // Set order (if the process has a parent, zero otherwise)
    if (LIKELY(0 != parent))
        proc->order = parent->order + 1;
    
    // Add to process list
    proc->next = process_list;
    process_list = proc;

    return proc;
}

process_t *process_get(uint32_t pid) {
    process_t *proc = process_list;

    while (0 != proc) {
        if (pid == proc->pid)
            return proc;

        proc = proc->next;
    }

    return 0;
}

void process_terminate(uint32_t pid) {
    // Find process (does not use process_get to be able to
    // remove to process from the list)
    process_t *proc = process_list;
    process_t *proc_prev = 0;

    while (0 != proc) {
        if (pid == proc->pid)
            break;

        proc_prev = proc;
        proc = proc->next;
    }

    // No such process
    if (0 != proc)
        process_list = proc->next;
    else
        proc_prev->next = proc->next;

    // Stop and free all threads
    thread_dispose_all(proc);

    // Free process structure
    heap_free(proc);
}
