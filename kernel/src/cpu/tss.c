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
#include <cpu.h>
#include <memory.h>
#include <debug.h>

extern uint8_t stack_runtime_top;
extern void cpu_tss_load(void);

/**
 * Creates and loads a Task State Segment.
 */
void cpu_tss_create(void) {
    // Create TSS
    cpu_tss_t *tss = (cpu_tss_t *) heap_alloc(sizeof(cpu_tss_t));
    memset((void *) tss, 0, sizeof(cpu_tss_t));

    tss->rsp0 = (uintptr_t) &stack_runtime_top;

    // Create TSS pointer in GDT
    uintptr_t tss_addr = (uintptr_t) tss;
    
    cpu_tss_ptr_t *ptr = (cpu_tss_ptr_t *) (MEMORY_GDT64_VADDR + 0xA + TSS_GDT_OFFSET);
    memset((void *) ptr, 0, sizeof(cpu_tss_ptr_t));

    // Set tss address as base
    ptr->baseLow = tss_addr & 0xFFFF;
    ptr->baseMiddleLow = (tss_addr >> 16) & 0xFF;
    ptr->baseMiddleHigh = (tss_addr >> 24) & 0xFF;
    ptr->baseHigh = (tss_addr >> 32) & 0xFFFFFFFF;
        
    // Set limit
    ptr->limitLow = sizeof(cpu_tss_t);
        
    // Set flags
    ptr->flags |=
        0x09 |      // TSS
        (1 << 7) |  // Present
        (3 << 5);   // Ring 3

    cpu_tss_load();
}
