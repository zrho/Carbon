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
#include <multitasking.h>
#include <memory.h>
#include <debug.h>

//- Processes ------------------------------------------------------------------

static process_t **process_map = (process_t **) MEMORY_PROCESS_MAP_VADDR;
process_t *process_list = 0;

static uint32_t _process_id_next(void) {
    // Search for first free id
    uint32_t id;

    for (id = 0; id < PROCESS_MAX; ++id)
        if (0 == process_map[id])
            return id;

    PANIC("Maximum number of processes reached.");
    return -1;
}

static void _process_create_thread_map(uint32_t pid) {
    // Map thread map
    uintptr_t thread_map = MEMORY_THREAD_MAP_VADDR + pid * THREAD_MAP_SIZE;
    uint16_t pflags = PAGE_FLAG_WRITEABLE | PAGE_FLAG_GLOBAL;
    size_t offset;

    for (offset = 0; offset < THREAD_MAP_SIZE; offset += 0x1000)
        memory_map(thread_map + offset, frame_alloc(), pflags);

    // Clear thread map
    memset((void *) thread_map, 0, THREAD_MAP_SIZE);
}

static void _process_dispose_thread_map(uint32_t pid) {
    // Unmap thread map
    uintptr_t thread_map = MEMORY_THREAD_MAP_VADDR + pid * THREAD_MAP_SIZE;
    size_t offset = 0;

    for (offset = 0; offset < THREAD_MAP_SIZE; offset += 0x1000) {
        uintptr_t phys = memory_physical(thread_map + offset);
        memory_unmap(thread_map + offset);
        frame_free(phys);
    }
}

void process_init(void) {
    // Map the thread map
    uintptr_t vaddr = MEMORY_PROCESS_MAP_VADDR;
    uint16_t pflags = PAGE_FLAG_GLOBAL | PAGE_FLAG_WRITEABLE;
    uintptr_t offset;

    for (offset = 0; offset < PROCESS_MAP_SIZE; offset += 0x1000)
        memory_map(vaddr + offset, frame_alloc(), pflags);

    // Clear the thread map
    memset((void *) vaddr, 0, PROCESS_MAP_SIZE);
}

process_t *process_spawn(uintptr_t addr_space, process_t *parent) {
    // Allocate process structure
    process_t *proc = (process_t *) heap_alloc(sizeof(process_t));
    memset(proc, 0, sizeof(process_t));

    // Fill structure
    proc->pid = _process_id_next();
    proc->threads = 0;
    proc->addr_space = addr_space;
    proc->stack_offset = 0;
    proc->parent = parent;

    // Create thread map
    _process_create_thread_map(proc->pid);

    // Link in map
    process_map[proc->pid] = proc;

    // Set order (if the process has a parent, zero otherwise)
    if (LIKELY(0 != parent))
        proc->order = parent->order + 1;
    
    // Add to process list
    proc->next = process_list;
    process_list = proc;

    return proc;
}

process_t *process_get(uint32_t pid) {
    if (pid >= PROCESS_MAX)
        return 0;

    return process_map[pid];
}

void process_terminate(uint32_t pid) {
    // Find process (does not use process_map to be able to
    // remove to process from the list)
    process_t *proc = process_list;
    process_t *proc_prev = 0;

    while (0 != proc) {
        if (pid == proc->pid)
            break;

        proc_prev = proc;
        proc = proc->next;
    }

    // No such process?
    if (0 == proc)
        return;

    // Remove from list
    if (0 == proc_prev)
        process_list = proc->next;
    else
        proc_prev->next = proc->next;

    // Dispose thread map
    _process_dispose_thread_map(pid);

    // Remove from map
    process_map[pid] = 0;

    // Stop and free all threads
    thread_dispose_all(proc);

    // Free process structure
    heap_free(proc);
}
