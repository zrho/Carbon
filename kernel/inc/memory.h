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
#include <api/bootinfo.h>
#include <api/map.h>

//- Physical Memory Management -------------------------------------------------

uintptr_t frame_alloc(void);
void frame_free(uintptr_t frame);
void frame_init(boot_info_t *info);
uint64_t frame_first_available(uint64_t from, boot_info_mmap_t *mmap);

//- Virtual Memory Management --------------------------------------------------

#define PAGE_FLAG_PRESENT (1 << 0)
#define PAGE_FLAG_WRITEABLE (1 << 1)
#define PAGE_FLAG_USER (1 << 2)
#define PAGE_FLAG_GLOBAL (1 << 8)

#define PAGE_STRUCT_PML4 4
#define PAGE_STRUCT_PDP  3
#define PAGE_STRUCT_PD   2
#define PAGE_STRUCT_PT   1

#define PAGE_SIZE 0x1000

void memory_map(uint64_t virtual_addr, uint64_t physical_addr, uint16_t flags);
void memory_unmap(uint64_t virtual_addr);
uint64_t memory_physical(uint64_t virtual_addr);
bool memory_user_accessible(uint64_t virtual_addr);

bool memory_region_accessible(uint64_t virtual_addr, uint64_t length);

uint64_t memory_struct_remove(uint64_t virtual_addr, uint8_t struct_idx);
void memory_struct_insert(uint64_t virtual_addr, uint8_t struct_idx, uint64_t struct_ptr);

//- Address Spaces -------------------------------------------------------------

uintptr_t memory_space_initial;

uintptr_t memory_space_get(void);
uintptr_t memory_space_switch(uintptr_t new_space);
uintptr_t memory_space_create(void);
void memory_space_dispose(void);

//- Heap -----------------------------------------------------------------------

#define HEAP_MAX_LENGTH (0x2000000000 - 0x8000)

void *heap_alloc(size_t size);
void heap_free(void *ptr);
uintptr_t heap_sbrk(intptr_t delta);
