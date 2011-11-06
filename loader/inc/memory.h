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

//- Frame Heap -----------------------------------------------------------------

uintptr_t frame_placement;

uintptr_t frame_alloc(void);

//- Mapping --------------------------------------------------------------------

#define PAGE_FLAG_PRESENT       (1 << 0)
#define PAGE_FLAG_WRITABLE      (1 << 1)
#define PAGE_FLAG_USER          (1 << 2)
#define PAGE_FLAG_GLOBAL        (1 << 8)

void memory_map_system(boot_info_t *info);
void memory_map(uint64_t virtual, uint64_t physical, uint16_t flags);
